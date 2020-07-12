#pragma once
#include <promise/promise_base.hpp>
#include <unique_handle.hpp>

namespace promise {
struct minimal : helper<minimal> {};
} // namespace promise
using minimal_coro = unique_handle<promise::minimal>;
