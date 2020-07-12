#pragma once
#include <conduit/promise/co_void.hpp>
#include <conduit/unique_handle.hpp>

namespace conduit {
using co_void = unique_handle<promise::co_void>;
}
