#pragma once
#include <common.hpp>

template <class Promise>
struct awaitable_promise_object {
    Promise& promise;
    constexpr bool await_ready() noexcept { return true; }
    constexpr void await_suspend(std::coroutine_handle<>) noexcept {}
    constexpr Promise* await_resume() noexcept { return &promise; }
};
