#pragma once
#include <conduit/mixin/promise_parts.hpp>

namespace conduit {
namespace promise {
struct coroutine;
}
struct coroutine {
    using promise_type = promise::coroutine;
};
} // namespace conduit

namespace conduit::promise {
// Represents a promise type for a coroutine that is coroutine
// and can't be resumed or checked for being done
struct coroutine : mixin::InitialSuspend<false>,
                   mixin::FinalSuspend<false>,
                   mixin::UnhandledException<>,
                   mixin::ReturnVoid {

    auto get_return_object() noexcept { return conduit::coroutine{}; }
};
} // namespace conduit::promise