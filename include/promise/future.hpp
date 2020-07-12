#pragma once
#include <async/callback.hpp>
#include <promise/promise_base.hpp>
#include <unique_handle.hpp>

namespace promise {
template <class ReturnValue>
struct future : helper<future<ReturnValue>> {
    std::coroutine_handle<> callback = std::noop_coroutine();
    ReturnValue returned_value = {};

    auto final_suspend() noexcept { return awaitable_callback{callback}; }
    void set_callback(std::coroutine_handle<> callback) noexcept { this->callback = callback; }
    void return_value(ReturnValue value) { returned_value = std::move(value); }
    ReturnValue get_value() { return std::move(returned_value); }
};
}
template <class T>
struct future : unique_handle<promise::future<T>> {
    using super = unique_handle<promise::future<T>>;
    using super::super;

    bool await_ready() noexcept { return super::done(); }
    auto await_suspend(std::coroutine_handle<> callback) noexcept {
        super::promise().set_callback(callback);
        return super::get_raw_handle();
    }
    auto await_resume() { return super::promise().get_value(); }
};
