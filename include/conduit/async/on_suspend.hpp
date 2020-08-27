#pragma once
#include <conduit/continuation.hpp>
#include <conduit/fn/bind.hpp>
#include <conduit/mixin/resumable.hpp>
#include <utility>

namespace conduit::async {
template <class F>
struct on_suspend_type : mixin::Resumable<on_suspend_type<F>> {
    [[no_unique_address]] F on_suspend;
};

template <class F>
on_suspend_type(int, F) -> on_suspend_type<F>;

template <class F, class... Args>
auto on_suspend(F&& f, Args&&... args) {
    if constexpr (sizeof...(Args) == 0) {
        return on_suspend_type{{}, std::forward<F>(f)};
    } else {
        return on_suspend_type{
            {}, fn::bind_last(std::forward<F>(f), std::forward<Args>(args)...)};
    }
}
} // namespace conduit::async