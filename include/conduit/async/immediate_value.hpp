#pragma once
#include <conduit/common.hpp>

namespace conduit::async {
template <class Value>
struct immediate_value {
    Value value;
    constexpr bool await_ready() noexcept { return true; }
    constexpr void await_suspend(std::coroutine_handle<>) noexcept {}
    constexpr Value await_resume() noexcept { return std::move(value); }
};
template<class Value>
immediate_value(Value) -> immediate_value<Value>;
} // namespace conduit
