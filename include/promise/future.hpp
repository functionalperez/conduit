#pragma once
#include <async/callback.hpp>
#include <promise/promise_base.hpp>

namespace promise {
template <class ReturnValue>
struct future : helper<future<ReturnValue>, suspend_initially> {
    std::coroutine_handle<> callback = std::noop_coroutine();
    ReturnValue returned_value = {};

    auto final_suspend() noexcept { return async::jump{callback}; }
    void set_callback(std::coroutine_handle<> callback) noexcept { this->callback = callback; }
    void return_value(ReturnValue value) { returned_value = std::move(value); }
    ReturnValue get_value() { return std::move(returned_value); }
};
}
