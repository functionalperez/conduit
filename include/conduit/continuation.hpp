#pragma once
#include <conduit/promise/helper.hpp>

namespace conduit::promise {
struct continuation;
}
namespace conduit {
struct continuation;
}

struct conduit::continuation : std::coroutine_handle<conduit::promise::continuation> {
    using base = std::coroutine_handle<conduit::promise::continuation>;
    using handle = std::coroutine_handle<conduit::promise::continuation>;
    using promise_type = conduit::promise::continuation;
    continuation(handle h) : base(h) {}
    using base::base;
    using base::operator bool;
};

// Represents a promise type for a coroutine that is void_coro
// and can't be resumed or checked for being done
struct conduit::promise::continuation : helper<continuation, traits<true, false, true, true>> {
    using handle = std::coroutine_handle<conduit::promise::continuation>;
    auto get_return_object() noexcept { return conduit::continuation{handle::from_promise(*this)}; }
    static auto get_return_object_on_allocation_failure() noexcept {
        return conduit::continuation{nullptr};
    }
};
