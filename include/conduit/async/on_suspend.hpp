#pragma once
#include <conduit/continuation.hpp>
#include <conduit/fn/bind.hpp>
#include <conduit/mixin/awaitable_parts.hpp>
#include <utility>

namespace conduit::async {
template <class F>
struct suspend_invoke {
    [[no_unique_address]] F on_suspend;
    constexpr bool await_ready() noexcept { return false; }
    auto await_suspend(std::coroutine_handle<> h) { return on_suspend(h); }
    void await_resume() {}
};
template<class F>
suspend_invoke(F) -> suspend_invoke<F>;

template <class F, class... Args>
auto on_suspend(F&& f, Args&&... args) {
    if constexpr (sizeof...(Args) == 0) {
        return suspend_invoke{std::forward<F>(f)};
    } else {
        return suspend_invoke{
            fn::bind_last(std::forward<F>(f), std::forward<Args>(args)...)};
    }
}
} // namespace conduit::async