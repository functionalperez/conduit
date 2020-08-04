#pragma once
#include <conduit/mixin/promise_parts.hpp>

namespace conduit::promise {

template <
    // Type output by generator
    class T,
    // Should funcitons be noexcept
    bool IsNoexcept,
    // Should the coroutine always suspend initially
    bool SuspendInitially>
struct generator
  : mixin::GetReturnObject<generator<T, IsNoexcept, SuspendInitially>>,
    mixin::InitialSuspend<SuspendInitially>,
    mixin::FinalSuspend<mixin::suspend::always>,
    mixin::UnhandledException<IsNoexcept>,
    mixin::ReturnVoid {

    // yielded value stored here
    T const* value;

    // Stores value in this->value, to be accessed by the caller via
    // coroutine_handle.promise().value
    constexpr auto yield_value(T const& v) noexcept {
        value = &v;
        return std::suspend_always{};
    }
};
} // namespace conduit::promise
