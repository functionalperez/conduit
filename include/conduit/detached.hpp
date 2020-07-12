#pragma once
#include <conduit/promise/detached.hpp>

namespace conduit {
// Represents a coroutine that is detached
class detached {
    bool started_successfully;
   public:
    detached(bool successful) : started_successfully(successful) {}
    using promise_type = promise::detached;
    operator bool() const { return started_successfully; }
};
} // namespace conduit
