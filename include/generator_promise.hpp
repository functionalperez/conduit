#pragma once
#include <common.hpp>

enum generator_mode : bool { check_first = false, resume_first = true };
template <
    // Type output by generator
    class T,
    // Template to make return object
    template <class> class ReturnObject_t,
    // Should funcitons be noexcept
    bool IsNoexcept = true,
    // Should the coroutine always suspend initially
    bool SuspendInitially = check_first>
struct generator_promise {
    using handle = std::coroutine_handle<generator_promise>;
    using return_object = ReturnObject_t<generator_promise>;

    // yielded value stored here
    T value;

    static auto get_return_object_on_allocation_failure() noexcept {
        return return_object{nullptr};
    }
    auto get_return_object() noexcept {
        return return_object{handle::from_promise(*this)};
    }
    constexpr auto initial_suspend() noexcept {
        if constexpr (suspend_initially) {
            return std::suspend_always();
        } else {
            return std::suspend_never();
        }
    }
    constexpr auto final_suspend() noexcept { return std::suspend_always(); }
    [[noreturn]] void unhandled_exception() noexcept { std::terminate(); }
    constexpr void return_void() noexcept {}
    constexpr auto yield_value(T value) noexcept(assign_T_noexcept) {
        this->value = std::move(value);
        return std::suspend_always{};
    }

    constexpr static bool is_noexcept = IsNoexcept;
    // true if initial_suspend() returns suspend_always
    constexpr static bool suspend_initially = SuspendInitially;

   private:
    // These are just used to get a reference to T in template expressions
    static T& mutable_T();
    static T const& const_T();
    constexpr static bool copy_T_noexcept =
        is_noexcept || noexcept(T(const_T()));
    constexpr static bool assign_T_noexcept =
        is_noexcept || noexcept(mutable_T() = const_T());
};
