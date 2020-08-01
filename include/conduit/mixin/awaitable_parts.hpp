#pragma once
#include <conduit/common.hpp>

namespace conduit::mixin {
template <bool ready>
struct AwaitReady {
    constexpr bool await_ready() const noexcept { return ready; }
};
struct AwaitSuspend {
    constexpr void await_suspend(std::coroutine_handle<>) const noexcept {}
};
struct AwaitResume {
    constexpr void await_resume() const noexcept {}
};
} // namespace conduit::mixin