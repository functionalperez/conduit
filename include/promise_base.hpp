#pragma once
#include <await_promise_object.hpp>
#include <common.hpp>
#include <unique_handle.hpp>

// This is an incomplete promise_base
template <bool SuspendInitially, bool HasReturnVoid = true>
struct promise_base_base {
    constexpr auto initial_suspend() noexcept {
        if constexpr (SuspendInitially)
            return std::suspend_always{};
        else
            return std::suspend_never{};
    }
    constexpr auto final_suspend() noexcept { return std::suspend_always(); }
    [[noreturn]] void unhandled_exception() noexcept { std::terminate(); }
    constexpr void return_void() noexcept {}
};
// This is an incomplete promise_base
template <bool SuspendInitially>
struct promise_base_base<SuspendInitially, false> {
    constexpr auto initial_suspend() noexcept {
        if constexpr (SuspendInitially)
            return std::suspend_always{};
        else
            return std::suspend_never{};
    }
    constexpr auto final_suspend() noexcept { return std::suspend_always(); }
    [[noreturn]] void unhandled_exception() noexcept { std::terminate(); }
};

// Implements a base type that encapsulates boilerplate code
// used to implement coroutine promise objects
template <
    // This is the class that derives from promise_base.
    class Promise,
    // And this determines whether or not initial_suspend returns
    // suspend_always or suspend_never
    bool SuspendInitially = true,
    // This determines whether or not promise_base has return_void
    bool HasReturnVoid = true>
struct promise_base : promise_base_base<SuspendInitially, HasReturnVoid> {
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
