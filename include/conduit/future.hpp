#pragma once
#include <conduit/async/coro.hpp>
#include <conduit/promise/future.hpp>

namespace conduit {
template<class T>
using future = async::coro<promise::future<T>>;

template<class T>
using optional_future = async::coro<promise::optional_future<T>>;
} // namespace conduit
