#pragma once
#include <conduit/stdlib_coroutine.hpp>

namespace conduit::async {
struct await_if {
    bool ready = false;
    constexpr bool await_ready() noexcept { return ready; }
    constexpr void await_suspend(std::coroutine_handle<>) noexcept {}
    constexpr void await_resume() noexcept {}
};
} // namespace conduit::async
