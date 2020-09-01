#pragma once
#include <conduit/fn/bind.hpp>
#include <conduit/util/stdlib_coroutine.hpp>
#include <utility>

namespace conduit::async {
template <class F>
struct on_suspend {
    [[no_unique_address]] F on_suspend;

    constexpr bool await_ready() noexcept { return false; }
    auto await_suspend(std::coroutine_handle<> h) { on_suspend(h); }
    void await_resume() {}
};
template <class F>
on_suspend(F) -> on_suspend<F>;
} // namespace conduit::async
