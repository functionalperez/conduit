#pragma once
#include "generator.hpp"
#include <awaitable_reference.hpp>

struct get_message_t{};
constexpr get_message_t get_message = {};

template <
    // Type output by generator
    class T,
    class Message,
    // Template to make return object
    template <class> class ReturnObject_t,
    // Should funcitons be noexcept
    bool IsNoexcept = true,
    // Should the coroutine always suspend initially
    bool SuspendInitially = check_first>
struct channel_promise {
    using handle = std::coroutine_handle<channel_promise>;
    using return_object = ReturnObject_t<channel_promise>;
    using message_type = Message;

    // yielded value stored here
    T value = {};
    // message stored here
    Message message = {};

    static auto get_return_object_on_allocation_failure() noexcept {
        return return_object{nullptr};
    }
    auto get_return_object() noexcept {
        return return_object{handle::from_promise(*this)};
    }
    constexpr auto initial_suspend() noexcept {
        if constexpr (SuspendInitially) {
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
        return awaitable_reference { message };
    }
    constexpr auto yield_value(get_message_t) {
        return awaitable_reference { message }; 
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