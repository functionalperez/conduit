#pragma once
#include <conduit/util/stdlib_coroutine.hpp>

namespace conduit::async {
template <class T>
struct deref {
    T& value;
    constexpr bool await_ready() noexcept { return false; }
    constexpr void await_suspend(std::coroutine_handle<>) noexcept {}
    constexpr T await_resume() noexcept(noexcept(T(value))) { return value; }
};
template <class T>
deref(T&) -> deref<T>;
} // namespace conduit::async
