#pragma once
#include <conduit/mixin/resumable.hpp>

namespace conduit::async {
// Calling co_await async::destroy{} will suspend and then destroy
// the current coroutine as well as any coroutines awaiting on it
struct destroy : mixin::Resumable<destroy> {
    void on_suspend(std::coroutine_handle<> caller) { caller.destroy(); }
};
} // namespace conduit::async