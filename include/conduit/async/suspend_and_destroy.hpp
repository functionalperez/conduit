#pragma once
#include <conduit/common.hpp>
#include <conduit/continuation.hpp>
#include <conduit/task.hpp>
#include <conduit/void_coro.hpp>

namespace conduit::async {
struct suspend_and_destroy {
    static continuation destroy_callback(std::coroutine_handle<> callback) {
        callback.destroy();
        co_return;
    }
    constexpr bool await_ready() { return false; }
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> callback) {
        return destroy_callback(callback);
    }
    void await_resume() {}
};
} // namespace conduit::async
