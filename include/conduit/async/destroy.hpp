#pragma once
#include <conduit/mixin/awaitable_parts.hpp>

namespace conduit::async {
// Calling co_await async::destroy{} will suspend and then destroy
// the current coroutine as well as any coroutines awaiting on it
struct destroy : mixin::AwaitReady<false>, mixin::AwaitResume {
    void await_suspend(std::coroutine_handle<> caller) { caller.destroy(); }
};
} // namespace conduit::async