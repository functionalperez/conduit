#pragma once
#include <conduit/async/on_coro.hpp>
#include <conduit/promise/source.hpp>
#include <conduit/unique_handle.hpp>

namespace conduit {
template <class T>
struct source : unique_handle<promise::source<T>> {
    using base = unique_handle<promise::source<T>>;
    using base::base;
    auto& operator co_await() & { return base::promise(); }
};
} // namespace conduit
