#pragma once
#include <async/deref.hpp>
#include <common.hpp>

namespace mini {

struct get_send_t {};
constexpr get_send_t get_send;
template <class Out, class In = Out>
struct channel {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        Out current_value;
        In response;
        async::deref<In> yield_value(get_send_t) {
            return async::deref(response);
        }
        async::deref<In> yield_value(Out value) {
            current_value = value;
            return async::deref(response);
        }

        channel get_return_object() {
            return {handle_type::from_promise(*this)};
        }

        // We run up until the first value is ready
        auto initial_suspend() noexcept { return std::suspend_never(); }

        auto final_suspend() noexcept { return std::suspend_always(); }

        void unhandled_exception() noexcept { std::terminate(); }

        void return_void() {}
    };

    Out receive() { return handle.promise().current_value; }

    void send(In response) {
        handle.promise().response = std::move(response);
        handle.resume();
    }

   private:
    channel(handle_type handle) : handle(handle) {}
    handle_type handle = nullptr;
};
} // namespace mini
