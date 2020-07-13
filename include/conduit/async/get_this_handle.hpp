#pragma once
#include <coroutine>

namespace conduit::async {
class get_this_handle {
   public:
    std::coroutine_handle<> handle;
    bool await_ready() { return true; }
    void await_suspend(std::coroutine_handle<> caller) { handle = caller; }
    auto await_resume() { return handle; }
};
} // namespace conduit::async
