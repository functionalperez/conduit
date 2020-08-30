#pragma once
#include <conduit/util/stdlib_coroutine.hpp>

namespace conduit::async {
struct jump {
    std::coroutine_handle<> callback = nullptr;
    constexpr bool await_ready() noexcept { return false; }
    constexpr auto await_suspend(std::coroutine_handle<>) noexcept {
        return callback;
    }
    constexpr void await_resume() noexcept {}
};
} // namespace conduit::async
