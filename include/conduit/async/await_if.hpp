#pragma once
#include <conduit/stdlib_coroutine.hpp>

namespace conduit::async {
struct await_if {
    bool not_ready = true;
    constexpr bool await_ready() noexcept { return !not_ready; }
    constexpr void await_suspend(std::coroutine_handle<>) noexcept {}
    constexpr void await_resume() noexcept {}
};
struct continue_if {
    bool ready = true;
    constexpr bool await_ready() noexcept { return ready; }
    constexpr void await_suspend(std::coroutine_handle<>) noexcept {}
    constexpr void await_resume() noexcept {}
};
} // namespace conduit::async
