#pragma once
#include <conduit/mixin/promise_parts.hpp>
#include <conduit/optional_ref.hpp>
#include <optional>

namespace conduit::promise {
template <class ReturnValue>
struct future : mixin::GetReturnObject<future<ReturnValue>>,
                mixin::PromiseWithCallback,
                mixin::UnhandledException<true> {
    std::optional<ReturnValue> result;
    void return_value(ReturnValue r) { result.emplace(std::move(r)); }
    void return_value(nothing_t) {}
    optional_ref<ReturnValue> get_value() & { return optional_ref(result); }
    std::optional<ReturnValue> get_value() && { return std::move(result); }
};
} // namespace conduit::promise
