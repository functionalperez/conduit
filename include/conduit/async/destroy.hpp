#pragma once
#include <conduit/mixin/resumable.hpp>

namespace conduit::async {
// Calling co_await async::destroy{} will suspend and then destroy
// the current coroutine as well as any coroutines awaiting on it
struct destroy : mixin::Resumable<destroy> {
    std::coroutine_handle<> caller;
    void set_caller(std::coroutine_handle<> h) { caller = h; }
    void resume() { caller.destroy(); }
};
} // namespace conduit::async