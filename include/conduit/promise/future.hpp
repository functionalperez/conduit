#pragma once
#include <conduit/async/jump.hpp>
#include <conduit/optional_ref.hpp>
#include <conduit/promise/helper.hpp>
#include <conduit/temporary_handle.hpp>

#include <optional>

namespace conduit::promise {
template <class ReturnValue>
struct future : helper<future<ReturnValue>, traits<true, true, false, true>> {
    temporary_handle callback = std::noop_coroutine();
    std::optional<ReturnValue> result;

    auto final_suspend() noexcept { return async::jump{callback.release()}; }
    void set_callback(std::coroutine_handle<> callback) noexcept { this->callback = callback; }
    void return_value(ReturnValue result) { this->result.emplace(std::move(result)); }
    void return_value(nothing_t) {}
    optional_ref<ReturnValue> get_value() & { return optional_ref(result); }
    std::optional<ReturnValue> get_value() && { return std::move(result); }
};
} // namespace conduit::promise
