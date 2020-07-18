#pragma once
#include <conduit/promise/helper.hpp>

namespace conduit::promise {
struct co_void : helper<co_void> {};
} // namespace conduit::promise
