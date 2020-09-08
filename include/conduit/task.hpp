#pragma once
#include <conduit/async/coro.hpp>
#include <conduit/mixin/promise_parts.hpp>
#include <stdexcept>

namespace conduit {
namespace promise {
struct task : mixin::GetReturnObject<task>,
              mixin::HasOwnerAndCallback,
              mixin::ReturnVoid {
    std::exception_ptr exception;
    void unhandled_exception() noexcept {
        exception = std::current_exception();
    }
    void get_value() {
        if (exception) {
            std::rethrow_exception(exception);
        }
    }
};
} // namespace promise

struct task : async::coro<promise::task> {
    using promise_type = promise::task;
    using async::coro<promise::task>::coro;
};
} // namespace conduit
