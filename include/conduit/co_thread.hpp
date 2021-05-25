#pragma once

#include <atomic>
#include <conduit/async/await_if.hpp>
#include <conduit/mixin/promise_parts.hpp>

namespace conduit {
class co_thread;
}
namespace conduit::promise {
class co_thread
  : public mixin::ExceptionHandler
  , public mixin::ReturnVoid {
    friend class conduit::co_thread;
    using coroutine_handle = std::coroutine_handle<co_thread>;
    auto get_handle() noexcept { return coroutine_handle::from_promise(*this); }
    constexpr static int state_paused = 0;
    constexpr static int state_running = 1;
    constexpr static int state_detached = 2;
    std::atomic_int state = state_running;

   public:
    struct pause_cothread {
        co_thread& h;
        constexpr bool await_ready() const noexcept {
            return h.state == state_detached;
        }
        bool await_suspend(std::coroutine_handle<>) noexcept {
            int expected = state_running;
            return h.state.compare_exchange_strong(expected, state_paused);
        }
        bool await_resume() {
            int expected = state_paused;
            return h.state.compare_exchange_strong(expected, state_running);
        }
    };
    constexpr co_thread* get_return_object() { return this; }
    constexpr auto final_suspend() noexcept { return pause_cothread {*this}; }
    constexpr pause_cothread initial_suspend() noexcept {
        return pause_cothread {*this};
    }
    constexpr pause_cothread await_transform(std::suspend_always) noexcept {
        return {*this};
    }
    template <class U>
    constexpr U&& await_transform(U&& u) noexcept {
        state = state_detached;
        return const_cast<U&&>(u);
    }
    ~co_thread() { ExceptionHandler::rethrow_if_exception(); }
};
} // namespace conduit::promise

namespace conduit {
class co_thread {
    using coroutine_handle = std::coroutine_handle<promise::co_thread>;
    promise::co_thread* thread;
    constexpr static int _paused = promise::co_thread::state_paused;
    constexpr static int _running = promise::co_thread::state_running;
    constexpr static int _detached = promise::co_thread::state_detached;

   public:
    using promise_type = promise::co_thread;
    // Atomically detaches the coroutine if it's running.
    // Otherwise, if it's paused, destroys the coroutine
    void destroy_if_paused() {
        int expected = _running;
        bool is_running =
            thread->state.compare_exchange_strong(expected, _detached);
        // If it's not running and it's not detached
        if (!is_running && expected == _paused) {
            thread->get_handle().destroy();
        }
    }
    co_thread() = default;
    co_thread(promise::co_thread* t) noexcept
      : thread {t} {}
    co_thread(co_thread&& h) noexcept
      : thread(h.thread) {
        h.thread = nullptr;
    }
    co_thread& operator=(co_thread t) { swap(t); return *this; }
    // Detaches the co_thread. If it's paused, resumes it.
    void detach() {
        int expected = _running;
        bool is_running =
            thread->state.compare_exchange_strong(expected, _detached);
        if (!is_running && expected == _paused) {
            thread->state = _detached;
            thread->get_handle().resume();
        }
        thread = nullptr;
    }
    void resume() {
        thread->get_handle().resume();
        if (thread->state == _paused) {
            thread->clear_and_rethrow_if_exception();
        }
    }
    constexpr bool is_detached() {
        return !thread;
    }
    bool is_running() {
        return thread->state == _running;
    }
    void operator()() { resume(); }
    constexpr bool has_thread() const noexcept { return thread; }
    constexpr operator bool() const noexcept { return thread; }
    ~co_thread() {
        if (has_thread())
            destroy_if_paused();
    }
    void swap(co_thread& other) { std::swap(thread, other.thread); }
};
} // namespace conduit
