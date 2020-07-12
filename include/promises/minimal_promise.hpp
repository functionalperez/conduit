#pragma once
#include <promises/promise_base.hpp>
#include <unique_handle.hpp>

struct minimal_promise : promise_base<minimal_promise> {};

using minimal_coro = unique_handle<minimal_promise>;
