#pragma once
#include <coroutine>

struct suspend_maybe {
    bool ready = false;
    constexpr bool await_ready() noexcept { return ready; }
    constexpr void await_suspend(std::coroutine_handle<>) {}
    constexpr void await_resume() {}
};
