#pragma once
#include <conduit/async/coro.hpp>
#include <conduit/mixin/promise_parts.hpp>
#include <conduit/util/optional_ref.hpp>
#include <stdexcept>
#include <variant>

namespace conduit::promise {
template <class ReturnValue>
struct future : mixin::GetReturnObject<future<ReturnValue>>,
                mixin::HasOwnerAndCallback {
    std::variant<ReturnValue, std::exception_ptr> result;
    struct visitor {
        auto operator()(ReturnValue& v) const -> ReturnValue {
            return std::move(v);
        }
        auto operator()(std::exception_ptr ptr) const -> ReturnValue {
            std::rethrow_exception(ptr);
        };
    };
    void unhandled_exception() { result = std::current_exception(); }
    void return_value(ReturnValue const& r) { result = r; }
    void return_value(ReturnValue&& r) { result = std::move(r); }
    ReturnValue get_value() { return std::visit(visitor{}, result); }
};

template <class ReturnValue>
struct optional_future
  : mixin::GetReturnObject<optional_future<ReturnValue>>,
    mixin::HasOwnerAndCallback,
    mixin::UnhandledException<optional_future<ReturnValue>> {
    std::variant<std::nullopt_t, ReturnValue, std::exception_ptr> result;
    struct visitor {
        auto operator()(ReturnValue& v) const -> std::optional<ReturnValue> {
            return {std::move(v)};
        }
        auto operator()(std::exception_ptr ptr) const
            -> std::optional<ReturnValue> {
            std::rethrow_exception(ptr);
        }
        auto operator()(std::nullopt_t) const -> std::optional<ReturnValue> {
            return std::nullopt;
        }
    };
    void return_value(ReturnValue const& r) { result = r; }
    void return_value(ReturnValue&& r) { result = std::move(r); }
    void return_value(std::optional<ReturnValue>&& opt) {
        if (opt)
            result = std::move(*opt);
        else
            result = std::nullopt;
    }
    void return_value(std::optional<ReturnValue> const& opt) {
        if (opt)
            result = *opt;
        else
            result = std::nullopt;
    }
    void return_value(tags::nothing_t) { result.emplace(std::nullopt); }
    void return_value(std::nullopt_t) { result.emplace(std::nullopt); }
    std::optional<ReturnValue> get_value() {
        return std::visit(visitor{}, result);
    }
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
