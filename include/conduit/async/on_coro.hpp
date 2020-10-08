#pragma once
#include <conduit/util/concepts.hpp>
#include <conduit/util/stdlib_coroutine.hpp>
#include <utility>

namespace conduit::async {
template <co_promise P, bool move_value = false>
struct on_coro {
    std::coroutine_handle<P> handle;

    constexpr bool await_ready() noexcept { return false; }
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiter) {
        handle.promise().set_callback(awaiter);
        return handle;
    }
    bool await_resume() noexcept { return !handle.done(); }
    auto await_resume() requires(has_get_value_member<P> && !move_value) {
        return handle.promise().get_value();
    }
    auto await_resume() requires(has_get_value_member<P>&& move_value) {
        return std::move(handle.promise()).get_value();
    }
};

} // namespace conduit::async
