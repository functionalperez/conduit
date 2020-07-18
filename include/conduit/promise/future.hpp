#pragma once
#include <conduit/async/jump.hpp>
#include <conduit/promise/helper.hpp>
#include <conduit/temporary_handle.hpp>

namespace conduit::promise {
template <class ReturnValue>
struct future : helper<future<ReturnValue>, suspend_initially> {
    temporary_handle callback = std::noop_coroutine();
    ReturnValue returned_value = {};

    auto final_suspend() noexcept { return async::jump{callback.release()}; }
    void set_callback(std::coroutine_handle<> callback) noexcept { this->callback = callback; }
    void return_value(ReturnValue value) { returned_value = std::move(value); }
    ReturnValue get_value() { return std::move(returned_value); }
};
} // namespace conduit::promise
