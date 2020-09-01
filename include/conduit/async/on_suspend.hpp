#pragma once
#include <conduit/fn/bind.hpp>
#include <conduit/mixin/awaitable_parts.hpp>
#include <utility>

namespace conduit::async {
template <class F>
struct suspend_invoke {
    [[no_unique_address]] F on_suspend;

    constexpr bool await_ready() noexcept { return false; }
    auto await_suspend(std::coroutine_handle<> h) { on_suspend(h); }
    void await_resume() {}
};
template <class F>
suspend_invoke(F) -> suspend_invoke<F>;

template <class F>
auto on_suspend(F&& f) -> suspend_invoke<F> {
    return {std::forward<F>(f)};
}
template <class F, class... Args>
auto on_suspend(F&& f, Args&&... args)
    -> suspend_invoke<decltype(fn::bind_last(std::forward<F>(f),
                                             std::forward<Args>(args)...))> {
    return {fn::bind_last(std::forward<F>(f), std::forward<Args>(args)...)};
}
} // namespace conduit::async