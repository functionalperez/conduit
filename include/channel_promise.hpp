#pragma once
#include <awaitable_reference.hpp>
#include <generator_promise.hpp>

template <
    // Type output by generator
    class T, class Message,
    // Template to make return object
    template <class> class ReturnObject,
    // Should funcitons be noexcept
    bool IsNoexcept = true,
    // Should the coroutine always suspend initially
    bool SuspendInitially = check_first>
struct channel_promise
    : promise_base<channel_promise<T, Message, ReturnObject, IsNoexcept, SuspendInitially>,
                   ReturnObject, SuspendInitially> {
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

    constexpr static bool is_noexcept = IsNoexcept;
    // true if initial_suspend() returns suspend_always
    constexpr static bool suspend_initially = SuspendInitially;

   private:
    // These are just used to get a reference to T in template expressions
    static T& mutable_T();
    static T&& moved_T();
    constexpr static bool move_T_noexcept = is_noexcept || noexcept(mutable_T() = moved_T());
};
