#pragma once
#include <coroutine>

namespace async {
struct await_if {
    bool ready = false;
    constexpr bool await_ready() noexcept { return ready; }
    constexpr void await_suspend(std::coroutine_handle<>) {}
    constexpr void await_resume() {}
};
} // namespace async
