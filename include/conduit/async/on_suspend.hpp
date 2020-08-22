#pragma once
#include <conduit/continuation.hpp>
#include <conduit/fn/bind.hpp>
#include <conduit/mixin/resumable.hpp>
#include <utility>

namespace conduit::async {
template <class Bind, class F>
concept on_suspend_bind = requires(Bind bind, F func,
                                   std::coroutine_handle<> h) {
    {bind(func, h)};
};

template <class F>
struct on_suspend : mixin::Resumable<on_suspend<F>> {
    F func;
    std::coroutine_handle<> caller;
    on_suspend(F const& func) : func(func) {}
    void set_caller(std::coroutine_handle<> handle) { caller = handle; }
    void resume() { func(caller); }
};

template <class F>
on_suspend(F) -> on_suspend<F>;

template <class F, class... Args>
auto make_on_suspend(F&& f, Args&&... args) {
    return on_suspend{
        fn::bind_last(std::forward<F>(f), std::forward<Args>(args)...)};
}
} // namespace conduit::async