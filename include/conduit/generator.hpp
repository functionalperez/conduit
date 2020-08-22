#pragma once
#include <conduit/mixin/promise_parts.hpp>
#include <conduit/util/iterator.hpp>
#include <conduit/util/unique_handle.hpp>

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

namespace conduit {
template <class T>
using generator = unique_handle<promise::generator<T>>;

template <class T>
auto begin(generator<T>& g)
    -> coro_iterator<std::coroutine_handle<promise::generator<T>>> {
    return {g.get()};
}
template <class T>
auto end(generator<T>& g)
    -> coro_iterator<std::coroutine_handle<promise::generator<T>>> {
    return {g.get()};
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
