#pragma once
#include <awaitable_reference.hpp>
#include <promises/generator_promise.hpp>

namespace promise {
template <
    // Type output by generator
    class T, class Message,
    // Should funcitons be noexcept
    bool IsNoexcept = true,
    
    bool InitialSuspend = false>
struct channel_promise
    : promise_base<channel_promise<T, Message, IsNoexcept>, promise_traits<InitialSuspend, true, true, IsNoexcept>> {
    using message_type = Message;

    // yielded value stored here
    T value = {};

    // message stored here
    Message message = {};

    constexpr auto yield_value(T value) noexcept(move_T_noexcept) {
        this->value = std::move(value);
        return awaitable_reference{message};
    }
    constexpr auto yield_value(get_message_t) { return awaitable_reference{message}; }

    // true if template noexcept flag is marked true
    constexpr static bool is_noexcept = IsNoexcept;

   private:
    // These are just used to get a reference to T in template expressions
    static T& mutable_T();
    static T&& moved_T();
    constexpr static bool move_T_noexcept = is_noexcept || noexcept(mutable_T() = moved_T());
};
}
