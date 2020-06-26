#pragma once
#include <common.hpp>

template <class Derived>
struct promise_base;

// This is an incomplete promise_base
template <>
struct promise_base<void> {
    constexpr auto initial_suspend() noexcept { return std::suspend_always(); }
    constexpr auto final_suspend() noexcept { return std::suspend_always(); }
    [[noreturn]] void unhandled_exception() noexcept { std::terminate(); }
    constexpr void return_void() noexcept {}
};

template <class Derived>
struct promise_base : promise_base<void> {
    using handle = typename Derived::handle;
    using return_object = typename Derived::return_object;

    Derived& get_promise() { return static_cast<Derived&>(*this); }
    static auto get_return_object_on_allocation_failure() noexcept {
        return return_object{nullptr};
    }
    auto get_return_object() noexcept {
        return return_object{handle::from_promise(get_promise())};
    }
};
