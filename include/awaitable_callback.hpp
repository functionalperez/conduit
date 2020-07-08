#pragma once
#include <common.hpp>

struct awaitable_callback {
    std::coroutine_handle<> callback = nullptr;
    constexpr bool await_ready() noexcept {
        return false;
    }
    std::coroutine_handle<> await_suspend() noexcept {
        return callback;
    }
    void await_resume() noexcept {}
};
