#pragma once
#include <conduit/util/stdlib_coroutine.hpp>

namespace conduit::async {
struct await_if {
    bool not_ready = true;
    constexpr inline bool await_ready() noexcept { return !not_ready; }
    inline void await_suspend(std::coroutine_handle<>) noexcept {}
    constexpr inline void await_resume() noexcept {}
};
struct continue_if {
    bool ready = true;
    constexpr inline bool await_ready() noexcept { return ready; }
    void await_suspend(std::coroutine_handle<>) noexcept {}
    constexpr inline void await_resume() noexcept {}
};
} // namespace conduit::async
