#pragma once
#include <conduit/util/stdlib_coroutine.hpp>
#include <utility>

namespace conduit::async {
template <class F>
struct on_suspend {
    [[no_unique_address]] F accept;

    constexpr bool await_ready() noexcept { return false; }
    auto await_suspend(std::coroutine_handle<> handle) { accept(handle); }
    void await_resume() {}
};
template <class F>
on_suspend(F) -> on_suspend<F>;
} // namespace conduit::async
