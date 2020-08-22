#pragma once
#include <conduit/util/stdlib_coroutine.hpp>
#include <utility>

namespace conduit::async {
template <class Value>
struct immediate_value {
    Value value;
    constexpr bool await_ready() noexcept { return true; }
    constexpr void await_suspend(std::coroutine_handle<>) noexcept {}
    constexpr Value await_resume() noexcept(noexcept(Value(std::move(value)))) { return std::move(value); }
};
template<class Value>
immediate_value(Value) -> immediate_value<Value>;
} // namespace conduit
