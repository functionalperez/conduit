#pragma once
#include <conduit/async/jump.hpp>
#include <conduit/optional_ref.hpp>
#include <conduit/promise/helper.hpp>
#include <conduit/temporary_handle.hpp>

#include <optional>

namespace conduit::promise {
template <class ReturnValue>
struct source : helper<source<ReturnValue>, traits<true, true, true, true>> {
    temporary_handle callback = std::noop_coroutine();
    std::optional<ReturnValue> result;

    auto final_suspend() noexcept { return async::jump{callback.release()}; }
    void set_callback(std::coroutine_handle<> callback) noexcept { this->callback = callback; }
    async::jump yield_value(ReturnValue value) {
        result.emplace(std::move(value));
        return async::jump{callback.release()};
    }
    void return_void() { result.reset(); }
    optional_ref<ReturnValue> get_value() & { return optional_ref(result); }
    std::optional<ReturnValue> get_value() && { return std::move(result); }
};
} // namespace conduit::promise
