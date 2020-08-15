#pragma once
#include <conduit/unique_handle.hpp>

namespace conduit::async {
template <class Promise, bool move_value = false>
struct on_coro {
    std::coroutine_handle<Promise> handle;

    constexpr bool await_ready() noexcept {
        return false;
    }
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiter) {
        handle.promise().set_callback(awaiter);
        return handle;
    }
    auto await_resume() {
        if constexpr(move_value) {
            return std::move(handle.promise()).get_value();
        } else {
            return handle.promise().get_value();
        }
    }
};

} // namespace conduit::async
