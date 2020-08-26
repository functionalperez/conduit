#pragma once
#include <conduit/async/coro.hpp>
#include <conduit/mixin/promise_parts.hpp>

namespace conduit {
namespace promise {
struct task : mixin::GetReturnObject<task>,
              mixin::HasOwnerAndCallback,
              mixin::UnhandledException<true>,
              mixin::ReturnVoid {};
} // namespace promise

struct task : async::coro<promise::task> {
    using promise_type = promise::task;
    using async::coro<promise::task>::coro;  
};
} // namespace conduit
