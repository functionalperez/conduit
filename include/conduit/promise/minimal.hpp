#pragma once
#include <conduit/promise/promise_base.hpp>
#include <conduit/unique_handle.hpp>

namespace conduit {
namespace promise {
struct minimal : helper<minimal> {};
} // namespace promise
using minimal_coro = unique_handle<promise::minimal>;
} // namespace conduit
