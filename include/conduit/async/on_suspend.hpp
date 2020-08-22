#pragma once
#include <conduit/continuation.hpp>
#include <conduit/fn/bind.hpp>
#include <conduit/mixin/awaitable_parts.hpp>
#include <utility>

namespace conduit::async {
template <class Bind, class F>
concept on_suspend_bind = requires(Bind bind, F func,
                                   std::coroutine_handle<> h) {
    {bind(func, h)};
};

template <class F, on_suspend_bind<F> Bind>
struct on_suspend : mixin::AwaitReady<false>, mixin::AwaitResume {
   private:
    struct package {
        [[no_unique_address]] F func;
        [[no_unique_address]] Bind bind;
        std::coroutine_handle<> handle = nullptr;
        void operator()() { bind(func, handle); }
    };

    mem::linda<package, noop_continuation_size> alloc;

   public:
    on_suspend(F const& func, Bind&& b)
      : alloc{{}, package{func, std::move(b)}} {}
    on_suspend(F&& func, Bind&& b)
      : alloc{{}, package{std::move(func), std::move(b)}} {}

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> caller) {
        alloc.callback.handle = caller;
        return noop_continuation(alloc);
    }
};

template <class F, on_suspend_bind<F> Bind>
on_suspend(F, Bind bind) -> on_suspend<F, Bind>;
} // namespace conduit::async