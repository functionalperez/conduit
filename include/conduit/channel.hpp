#pragma once
#include <conduit/coroutine_queue.hpp>
#include <conduit/scheduler.hpp>
#include <utility>

namespace conduit {
template <class T, class Executor = scheduler>
class channel {
    Executor& context;
    coroutine_queue awaiting_send;
    coroutine_queue awaiting_recieve;

   public:
    channel(Executor& ex) noexcept : context(ex) {}
    struct await_send;
    struct await_recieve;

    Executor& executor() const { return context; }
    struct await_send : coroutine_node {
        channel& ch;
        [[no_unqiue_address]] T value;

        inline bool await_ready() {
            if (ch.awaiting_send.empty()) {
                await_recieve* recv =
                    static_cast<await_recieve*>(ch.awaiting_recieve.dequeue());
                if (recv) {
                    recv->value = std::move(value);
                    ch.context.post(*recv);
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        inline void await_suspend(std::coroutine_handle<> h) {
            caller = h;
            ch.awaiting_send.enqueue(*this);
        }
        inline void await_resume() {}
    };
    struct await_recieve : coroutine_node {
        channel& ch;
        [[no_unqiue_address]] T value;
        inline bool await_ready() {
            if (ch.awaiting_recieve.empty()) {
                await_send* send =
                    static_cast<await_send*>(ch.awaiting_send.dequeue());
                if (send) {
                    value = std::move(send->value);
                    ch.context.post(*send);
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
        inline void await_suspend(std::coroutine_handle<> h) {
            caller = h;
            ch.awaiting_recieve.enqueue(*this);
        }
        inline T await_resume() { return std::move(value); }
    };
    [[nodiscard]] await_send send(T const& value) {
        return await_send{{}, *this, value};
    }
    [[nodiscard]] await_send send(T&& value) {
        return await_send{{}, *this, std::move(value)};
    }
    await_recieve operator co_await() & { return await_recieve{{}, *this}; }
};

template <class Executor = scheduler>
class wait_notify {
    Executor& context;
    coroutine_queue awaiting_notify;
    std::atomic_int pending = 0;

   public:
    wait_notify(Executor& ex) noexcept : context(ex) {}

    Executor& executor() const { return context; }
    struct await_notify : coroutine_node {
        wait_notify<Executor>& ch;
        inline bool await_ready() noexcept {
            if (ch.awaiting_notify.empty()) {
                if (ch.pending-- > 0) {
                    return true;
                } else {
                    ch.pending++;
                    return false;
                }
            } else {
                return false;
            }
        }
        inline void await_suspend(std::coroutine_handle<> h) {
            caller = h;
            ch.awaiting_notify.enqueue(*this);
            if (ch.pending-- > 0) {
                if (coroutine_node* coro = ch.awaiting_notify.dequeue()) {
                    ch.context.post(*coro);
                }
            } else {
                ch.pending++;
            }
        }
        inline void await_resume() noexcept {}
    };
    void notify_one() {
        if (coroutine_node* coro = awaiting_notify.dequeue()) {
            context.post(*coro);
        } else {
            pending.fetch_add(1);
        }
    }
    await_notify operator co_await() & { return await_notify{{}, *this}; }
};
} // namespace conduit
