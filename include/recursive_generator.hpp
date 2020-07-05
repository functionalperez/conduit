#pragma once
#include <iostream>
#include <promise_base.hpp>
#include <suspend_maybe.hpp>
#include <unique_handle.hpp>

template <
    // Type output by generator
    class T,
    // Should funcitons be noexcept
    bool IsNoexcept = true,
    // Should the coroutine always suspend initially
    bool SuspendInitially = check_first>
struct recursive_generator_promise
    : promise_base<recursive_generator_promise<T, IsNoexcept, SuspendInitially>, unique_handle,
                   SuspendInitially> {

    using base_type = promise_base<recursive_generator_promise<T, IsNoexcept, SuspendInitially>,
                                   unique_handle, SuspendInitially>;
    using return_object = typename base_type::return_object;

   private:
    return_object* sauce = nullptr;

   public:
    void set_return_object(return_object* sauce) {
        this->sauce = sauce;
        std::cout << "Obtained return object of " << (void*)sauce << '\n';
    }

    void return_value(return_object new_generator) {
        *sauce = std::move(new_generator);
    }

    // suspend_maybe final_suspend() noexcept {
    //     // We suspend if the sauce isn't null
    //     // To allow the sauce to clean it up
    //     return suspend_maybe{sauce != nullptr};
    // }
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

template <class T>
using recursive_generator = unique_handle<recursive_generator_promise<T>>;
