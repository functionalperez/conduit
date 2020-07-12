#pragma once
#include <promises/promise_base.hpp>
#include <unique_handle.hpp>

namespace promise {
struct minimal : helper<minimal> {};
}
using minimal_coro = unique_handle<promise::minimal>;
