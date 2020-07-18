#pragma once
#include <conduit/promise/helper.hpp>
#include <conduit/temporary_handle.hpp>
#include <conduit/async/jump.hpp>

namespace conduit {
namespace promise {
struct task;
} // namespace promise

struct task : unique_handle<promise::task> {
    using unique_handle<promise::task>::unique_handle;
};

namespace promise {
struct task : helper<task, traits<true, true, true, true>> {
    using base = promise::helper<task, traits<true, true, true, true>>;
    conduit::task get_return_object() { return conduit::task{*this}; }
    auto final_suspend() noexcept { return async::jump{callback.release()}; }
    temporary_handle callback = std::noop_coroutine();
    void set_callback(std::coroutine_handle<> callback) noexcept { this->callback = callback; }
};
}
} // namespace conduit
