#pragma once
#include <conduit/promise/helper.hpp>

namespace conduit::promise {
struct void_coro;
}
namespace conduit {
struct void_coro;
}

struct conduit::void_coro {
    bool good;

    void_coro(bool good) : good(good) {}
    using promise_type = conduit::promise::void_coro;
    operator bool() const { return good; }
};

// Represents a promise type for a coroutine that is void_coro
// and can't be resumed or checked for being done
struct conduit::promise::void_coro : helper<void_coro, traits<false, false, true, true>> {

    auto get_return_object() noexcept { return conduit::void_coro{true}; }
    static auto get_return_object_on_allocation_failure() noexcept {
        return conduit::void_coro{false};
    }
};
