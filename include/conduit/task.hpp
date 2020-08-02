#pragma once
#include <conduit/async/on_coro.hpp>
#include <conduit/mixin/promise_parts.hpp>

namespace conduit {
namespace promise {
struct task : mixin::GetReturnObject<task>,
              mixin::PromiseWithCallback,
              mixin::UnhandledException<true>,
              mixin::ReturnVoid {};
} // namespace promise

struct task : unique_handle<promise::task> {
    using unique_handle<promise::task>::unique_handle;
    auto operator co_await() & { return async::on_coro{*this}; }
    auto operator co_await() && { return async::on_coro{std::move(*this)}; }
};
} // namespace conduit
