#pragma once
#include <conduit/mixin/promise_parts.hpp>

namespace conduit::promise {

template <class T>
struct generator : mixin::GetReturnObject<generator<T>>,
                   mixin::InitialSuspend<false>,
                   mixin::FinalSuspend<true>,
                   mixin::UnhandledException<>,
                   mixin::ReturnVoid {
   private:
    // yielded value stored here
    T const* pointer;

   public:
    constexpr T const& value() const noexcept { return *pointer; }
    // Stores value in this->value, to be accessed by the caller via
    // coroutine_handle.promise().value
    constexpr auto yield_value(T const& v) noexcept {
        pointer = &v;
        return std::suspend_always{};
    }
};
} // namespace conduit::promise
