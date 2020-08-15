#pragma once
#include <conduit/async/coro.hpp>
#include <conduit/promise/source.hpp>
#include <conduit/unique_handle.hpp>

namespace conduit {
template<class T>
using source = async::coro<promise::source<T>>;
} // namespace conduit
