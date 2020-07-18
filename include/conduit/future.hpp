#pragma once
#include <conduit/async/on_coro.hpp>
#include <conduit/promise/future.hpp>
#include <conduit/unique_handle.hpp>

namespace conduit {
template <class T>
struct future : unique_handle<promise::future<T>> {
    using super = unique_handle<promise::future<T>>;
    using super::super;

    auto operator co_await() & { return async::on_coro{*this}; }
    auto operator co_await() && { return async::on_coro{std::move(*this)}; }
};
} // namespace conduit
