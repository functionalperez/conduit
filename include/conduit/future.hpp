#pragma once
#include <conduit/async/coro.hpp>
#include <conduit/mixin/promise_parts.hpp>
#include <conduit/util/optional_ref.hpp>
#include <optional>

namespace conduit::promise {
template <class ReturnValue>
struct future : mixin::GetReturnObject<future<ReturnValue>>,
                mixin::HasOwnerAndCallback,
                mixin::UnhandledException<future<ReturnValue>> {
    std::optional<ReturnValue> result;
    void return_value(ReturnValue const& r) { result.emplace(r); }
    void return_value(ReturnValue&& r) { result.emplace(std::move(r)); }
    ReturnValue get_value() & { return std::move(*result); }
    ReturnValue get_value() && { return std::move(*result); }
};

template <class ReturnValue>
struct optional_future : mixin::GetReturnObject<optional_future<ReturnValue>>,
                         mixin::HasOwnerAndCallback,
                         mixin::UnhandledException<optional_future<ReturnValue>> {
    std::optional<ReturnValue> result;

    void return_value(ReturnValue const& r) { result.emplace(r); }
    void return_value(ReturnValue&& r) { result.emplace(std::move(r)); }
    void return_value(tags::nothing_t) {}
    std::optional<ReturnValue> get_value() & { return std::move(result); }
    std::optional<ReturnValue> get_value() && { return std::move(result); }
};
} // namespace conduit::promise

namespace conduit {
template <class T>
struct future : async::coro<promise::future<T>> {
    using promise_type = promise::future<T>;
    using async::coro<promise::future<T>>::coro;
};

template <class T>
struct optional_future : async::coro<promise::optional_future<T>> {
    using promise_type = promise::optional_future<T>;
    using async::coro<promise::optional_future<T>>::coro;
};
} // namespace conduit
