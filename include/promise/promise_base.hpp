#pragma once
#include <await_promise_object.hpp>
#include <common.hpp>
#include <unique_handle.hpp>

namespace promise {
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
template <bool IsNoexcept = true>
struct unhandled_exception_base {
    [[noreturn]] void unhandled_exception() noexcept { std::terminate(); }
};
template <>
struct unhandled_exception_base<false> {
    void unhandled_exception() noexcept {}
};
template <bool HasReturnVoid = true>
struct return_void_base {
    constexpr void return_void() noexcept {}
};
template <>
struct return_void_base<false> {};

template<bool initial_suspend = false, bool final_suspend = true, bool needs_return = true, bool noexcept_ = true>
struct traits {
    constexpr static bool suspends_initially = initial_suspend;
    constexpr static bool suspends_finally = final_suspend;
    constexpr static bool needs_return_void = needs_return;
    constexpr static bool is_noexcept = noexcept_;
};
using no_return_void = traits<false, true, false, true>;

constexpr traits<> get_traits(auto const& promise);

// Implements a base type that encapsulates boilerplate code
// used to implement coroutine promise objects
template <
    // This is the class that derives from promise_base.
    class Promise, 
    class traits = decltype(get_traits(std::declval<Promise>()))>
struct helper : initial_suspend_base<traits::suspends_initially>,
                      final_suspend_base<traits::suspends_finally>,
                      return_void_base<traits::needs_return_void>,
                      unhandled_exception_base<traits::is_noexcept> {

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
}
