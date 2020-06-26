#pragma once
#include <promise_base.hpp>

enum generator_mode : bool { check_first = false, resume_first = true };
template <
    // Type output by generator
    class T,
    // Template to make return object
    template <class> class ReturnObject,
    // Should funcitons be noexcept
    bool IsNoexcept = true,
    // Should the coroutine always suspend initially
    bool SuspendInitially = check_first>
struct generator_promise
    : promise_base<generator_promise<T, ReturnObject, IsNoexcept, SuspendInitially>, ReturnObject> {
    // yielded value stored here
    T value;

    // If SuspendsInitially is false, we hide the default behavior in
    // promise_base
    constexpr auto initial_suspend() noexcept {
        if constexpr (SuspendInitially)
            return std::suspend_always{};
        else
            return std::suspend_never{};
    }

    // Stores value in this->value, to be accessed by the caller via
    // coroutine_handle.promise()
    constexpr auto yield_value(T value) noexcept(move_T_noexcept) {
        this->value = std::move(value);
        return std::suspend_always{};
    }

    // true if template noexcept flag is marked true
    constexpr static bool is_noexcept = IsNoexcept;

    // true if initial_suspend() returns suspend_always
    constexpr static bool suspend_initially = SuspendInitially;

   private:
    // These are just used to get a reference to T in template expressions
    static T& mutable_T();
    static T&& moved_T();
    constexpr static bool move_T_noexcept = is_noexcept || noexcept(mutable_T() = moved_T());
};
