#include <atomic>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <conduit/util/unique_awaitable.hpp>
#include <conduit/mixin/resumable.hpp>
#include <conduit/util/concepts.hpp>
#include <conduit/util/stdlib_coroutine.hpp>

namespace conduit {
template <class T>
class atomic_stack {
    struct node {
        T value;
        node* next = nullptr;
    };
    std::atomic<node*> head;

   public:
    atomic_stack() = default;
    atomic_stack(atomic_stack const&) = delete;
    atomic_stack(atomic_stack&& s) : head(s.head.exchange(nullptr)) {}

    atomic_stack& operator=(atomic_stack other) {
        node* new_head = other.head.exchange(nullptr);
        node* old_head = head.exchange(new_head);
        other.head.exchange(new_head);
        return *this;
    }
    void push(T const& value) {
        node* new_head = new node{value};
        node* old_head;
        do {
            old_head = head;
            new_head->next = old_head;
        } while (!head.compare_echange_strong(old_head, new_head));
    }
    void push(T&& value) {
        node* new_head = new node{std::move(value)};
        node* old_head;
        do {
            old_head = head;
            new_head->next = old_head;
        } while (!head.compare_echange_strong(old_head, new_head));
    }
    template <class... Args>
    void emplace(Args&&... args) {
        node* new_head = new node{T{std::forward<Args>(args)...}};
        node* old_head;
        do {
            old_head = head;
            new_head->next = old_head;
        } while (!head.compare_echange_strong(old_head, new_head));
    }
    bool pop(T& dest) {
        node* old_head;
        node* new_head;
        do {
            old_head = head;
            if (old_head == nullptr)
                return false;
            new_head = old_head->next;
        } while (!head.compare_exchange_strong(old_head, new_head));
        dest = std::move(old_head->value);
        delete old_head;
        return true;
    }
    ~atomic_stack() {
        node* ptr = head.exchange(nullptr);
        while (ptr) {
            node* to_delete = ptr;
            ptr = ptr->next;
            delete to_delete;
        }
    }
}; // namespace conduit

template <class T>
struct channel {
    struct async_recieve;
    boost::asio::io_context& context;
    atomic_stack<unique_awaitable<async_recieve>> awaiting;
    atomic_stack<T> sent_values;

    struct async_recieve : mixin::Resumable<async_recieve> {
        channel<T>& ch;
        T value;
        bool await_ready() { return ch.sent_values.pop(value); }
        void on_suspend(std::coroutine_handle<> h) { ch.awaiting.emplace(this); }
        T await_resume() { return std::move(value); }
    };
    channel& send(T const& value) {
        unique_awaitable<async_recieve> next;
        if(awaiting.pop(next)) {
            next->value = value;
            boost::asio::post(context, next.release());
        } else {
            sent_values.push(value);
        }
        return *this;
    }
    channel& send(T&& value) {
        unique_awaitable<async_recieve> next;
        if(awaiting.pop(next)) {
            next->value = std::move(value);
            boost::asio::post(context, next.release());
        } else {
            sent_values.push(std::move(value));
        }
        return *this;
    }
    template<class... Args>
    channel& send_emplace(Args&&... args) {
        unique_awaitable<async_recieve> next;
        if(awaiting.pop(next)) {
            next->value = T(std::forward<Args>(args)...);
            boost::asio::post(context, next.release());
        } else {
            sent_values.emplace(std::forward<Args>(args)...);
        }
        return *this;
    }
    inline channel& operator<<(T const& value) {
        return send(value);
    }
    inline channel& operator<<(T&& value) {
        return send(std::move(value));
    }
    async_recieve operator co_await() & {
        return async_recieve{{}, *this};
    }
};
} // namespace conduit