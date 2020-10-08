#pragma once
#include <conduit/mixin/promise_parts.hpp>
#include <conduit/util/iterator.hpp>
#include <conduit/util/unique_handle.hpp>

namespace conduit::promise {
template <class T>
struct generator
  : mixin::GetReturnObject<generator<T>>
  , mixin::InitialSuspend<false>
  , mixin::FinalSuspend<true>
  , mixin::UnhandledException<generator<T>>
  , mixin::ReturnVoid {
   public:
    T value;
    // Stores value in this->value, to be accessed by the caller via
    // coroutine_handle.promise().value
    constexpr auto
    yield_value(T const& v) noexcept(std::is_nothrow_copy_assignable_v<T>) {
        value = v;
        return std::suspend_always {};
    }
    constexpr auto
    yield_value(T&& v) noexcept(std::is_nothrow_move_assignable_v<T>) {
        value = std::move(v);
        return std::suspend_always {};
    }
};
} // namespace conduit::promise

namespace conduit {
template <class T>
struct generator : unique_handle<promise::generator<T>> {
    using promise_type = promise::generator<T>;
    using unique_handle<promise::generator<T>>::unique_handle;
    using unique_handle<promise::generator<T>>::promise;
};

template <class T>
auto begin(generator<T>& g)
    -> coro_iterator<std::coroutine_handle<promise::generator<T>>> {
    return {g.promise()};
}
template <class T>
auto end(generator<T>& g)
    -> coro_iterator<std::coroutine_handle<promise::generator<T>>> {
    return {g.promise()};
}

template <class T>
bool operator>>(generator<T>& g, T& value) {
    if (g.done())
        return false;
    value = g->value;
    g.resume();
    return true;
}
} // namespace conduit
