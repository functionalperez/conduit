#pragma once
#include <conduit/mixin/promise_parts.hpp>
#include <conduit/optional_ref.hpp>

#include <optional>

namespace conduit::promise {
template <class ReturnValue>
struct source : mixin::GetReturnObject<source<ReturnValue>>,
                mixin::PromiseWithCallback,
                mixin::UnhandledException<true> {
    std::optional<ReturnValue> result;

    async::jump yield_value(ReturnValue value) {
        result.emplace(std::move(value));
        return callback.release_jump();
    }
    void return_void() { result.reset(); }
    optional_ref<ReturnValue> get_value() & { return optional_ref(result); }
    std::optional<ReturnValue> get_value() && { return std::move(result); }
};
} // namespace conduit::promise
