#pragma once
#include <conduit/promise/promise_base.hpp>

namespace conduit::promise {
struct co_void : helper<co_void> {};
} // namespace conduit::promise
