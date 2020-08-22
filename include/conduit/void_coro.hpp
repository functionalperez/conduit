#pragma once
#include <conduit/mixin/promise_parts.hpp>

namespace conduit {
namespace promise {
struct void_coro;
}
struct void_coro {
    using promise_type = promise::void_coro;
};
} // namespace conduit

namespace conduit::promise {
// Represents a promise type for a coroutine that is void_coro
// and can't be resumed or checked for being done
struct void_coro : mixin::InitialSuspend<false>,
                   mixin::FinalSuspend<false>,
                   mixin::UnhandledException<>,
                   mixin::ReturnVoid {

    auto get_return_object() noexcept { return conduit::void_coro{}; }
};
} // namespace conduit::promise