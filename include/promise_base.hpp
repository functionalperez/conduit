#pragma once
#include <await_promise_object.hpp>
#include <common.hpp>
#include <unique_handle.hpp>

template <bool suspend>
struct initial_suspend_base {
    constexpr auto initial_suspend() noexcept {
        if constexpr (suspend) {
            return std::suspend_always{};
        } else {
            return std::suspend_never{};
        }
    }
};
template <bool suspend>
struct final_suspend_base {
    constexpr auto final_suspend() noexcept {
        if constexpr (suspend) {
            return std::suspend_always{};
        } else {
            return std::suspend_never{};
        }
    }
};
struct unhandled_exception_terminate_base {
    [[noreturn]] void unhandled_exception() noexcept { std::terminate(); }
};
template <bool HasReturnVoid = true>
struct return_void_base {
    constexpr void return_void() noexcept {}
};
template <>
struct return_void_base<false> {};

// Implements a base type that encapsulates boilerplate code
// used to implement coroutine promise objects
template <
    // This is the class that derives from promise_base.
    class Promise,
    // And this determines whether or not initial_suspend returns
    // suspend_always or suspend_never
    bool SuspendInitially = true,
    // Determines what final_suspend returns
    bool SuspendFinally = true,
    // This determines whether or not promise_base has return_void
    bool HasReturnVoid = true>
struct promise_base : initial_suspend_base<SuspendInitially>,
                      final_suspend_base<SuspendFinally>,
                      return_void_base<HasReturnVoid>,
                      unhandled_exception_terminate_base {
    using handle_type = std::coroutine_handle<Promise>;

    // If there's an allocation failure, returns a null coroutine handle
    static handle_type get_return_object_on_allocation_failure() noexcept { return nullptr; }

    // Allows you access to the promise object from within a coroutine via
    // auto& promise = co_yield get_promise;
    // await_ready() always returns true
    awaitable_promise_object<Promise> yield_value(get_promise_t) {
        return awaitable_promise_object<Promise>{static_cast<Promise&>(*this)};
    }

    // Used by the compiler to produce the return_object when starting the coroutine
    handle_type get_return_object() noexcept { return get_handle(); }

    handle_type get_handle() { return handle_type::from_promise(static_cast<Promise&>(*this)); }
};
