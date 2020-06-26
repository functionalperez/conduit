#pragma once
#include <promise_base.hpp>
#include <unique_handle.hpp>

struct minimal_promise : promise_base<minimal_promise> {};

using minimal_coro = typename minimal_promise::return_object;
