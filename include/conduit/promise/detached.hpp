#pragma once
#include <conduit/promise/promise_base.hpp>

namespace conduit::promise {
// Represents a promise type for a coroutine that is detached
// and can't be resumed or checked for being done
struct detached : helper<detached, traits<false, false, true, true>> {
    using super = helper<detached, traits<false, false, true, true>>;

    using super::yield_value;
    
    constexpr bool get_return_object() noexcept {
        return true;
    }
    constexpr static bool get_return_object_on_allocation_failure() noexcept {
        return false;
    }
};
} // namespace conduit::promise
