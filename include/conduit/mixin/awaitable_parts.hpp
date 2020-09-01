#pragma once
#include <conduit/util/stdlib_coroutine.hpp>

namespace conduit::mixin {
template <bool ready>
struct AwaitReady {
    constexpr inline bool await_ready() const noexcept { return ready; }
};
struct AwaitSuspend {
    inline void await_suspend(std::coroutine_handle<>) const noexcept {}
};
struct AwaitResume {
    constexpr inline void await_resume() const noexcept {}
};
} // namespace conduit::mixin