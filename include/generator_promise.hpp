#pragma once
#include <promise_base.hpp>

template <
    // Type output by generator
    class T,
    // Should funcitons be noexcept
    bool IsNoexcept = true,
    // Should the coroutine always suspend initially
    bool SuspendInitially = check_first>
struct generator_promise;

template<class T, bool is_noexcept, bool suspends_initially>
constexpr promise_traits<suspends_initially, true, true, is_noexcept> is_promise_noexcept(generator_promise<T, is_noexcept, suspends_initially> const&);

template <
    // Type output by generator
    class T,
    // Should funcitons be noexcept
    bool IsNoexcept,
    // Should the coroutine always suspend initially
    bool SuspendInitially>
struct generator_promise
    : promise_base<generator_promise<T, IsNoexcept, SuspendInitially>> {

    // yielded value stored here
    T value;

    // Stores value in this->value, to be accessed by the caller via
    // coroutine_handle.promise().value
    constexpr auto yield_value(T value) noexcept(move_T_noexcept) {
        this->value = std::move(value);
        return std::suspend_always{};
    }

    // true if template noexcept flag is marked true
    constexpr static bool is_noexcept = IsNoexcept;

   private:
    // These are just used to get a reference to T in template expressions
    static T& mutable_T();
    static T&& moved_T();
    constexpr static bool move_T_noexcept = is_noexcept || noexcept(mutable_T() = moved_T());
};
