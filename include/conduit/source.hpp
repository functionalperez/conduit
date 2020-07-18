#pragma once
#include <conduit/async/on_coro.hpp>
#include <conduit/promise/source.hpp>
#include <conduit/unique_handle.hpp>

namespace conduit {
template <class T>
struct source : unique_handle<promise::source<T>> {
    using base = unique_handle<promise::source<T>>;
    using base::base;
    async::on_coro<promise::source<T>> value() & { return async::on_coro{*this}; }
    auto value() && { return async::on_coro{std::move(*this)}; }
};
} // namespace conduit
