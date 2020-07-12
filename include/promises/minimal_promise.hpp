#pragma once
#include <promises/promise_base.hpp>
#include <unique_handle.hpp>

namespace promise {
struct minimal_promise : promise_base<minimal_promise> {};
}
using minimal_coro = unique_handle<promise::minimal_promise>;
