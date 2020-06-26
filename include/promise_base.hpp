#pragma once
#include <common.hpp>
#include <unique_handle.hpp>

// This is an incomplete promise_base
struct promise_base_base {
    constexpr auto initial_suspend() noexcept { return std::suspend_always(); }
    constexpr auto final_suspend() noexcept { return std::suspend_always(); }
    [[noreturn]] void unhandled_exception() noexcept { std::terminate(); }
    constexpr void return_void() noexcept {}
};

template <class Derived, template <class> class ReturnObject = unique_handle>
struct promise_base : promise_base_base {
    using handle = std::coroutine_handle<Derived>;
    using return_object = ReturnObject<Derived>;

    Derived& get_promise() { return static_cast<Derived&>(*this); }
    static auto get_return_object_on_allocation_failure() noexcept {
        return return_object{nullptr};
    }
    auto get_return_object() noexcept {
        return return_object{handle::from_promise(get_promise())};
    }
};
