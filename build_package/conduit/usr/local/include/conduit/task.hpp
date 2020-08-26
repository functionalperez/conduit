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

using task = async::coro<promise::task>;
} // namespace conduit
