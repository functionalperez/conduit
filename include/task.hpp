#pragma once
#include <awaitable_callback.hpp>
#include <promise_base.hpp>
#include <unique_handle.hpp>

template <class ReturnValue>
struct future_promise : promise_base<future_promise<ReturnValue>, true, false> {
    std::coroutine_handle<> callback = std::noop_coroutine();
    ReturnValue returned_value = {};

    auto final_suspend() noexcept { return awaitable_callback{callback}; }
    void set_callback(std::coroutine_handle<> callback) noexcept { this->callback = callback; }
    void return_value(ReturnValue value) { returned_value = std::move(value); }
    ReturnValue get_value() { return std::move(returned_value); }
};

template <class T>
struct future : unique_handle<future_promise<T>> {
    using base_type = unique_handle<future_promise<T>>;
    using base_type::base_type;

    bool await_ready() noexcept { return base_type::done(); }
    auto await_suspend(std::coroutine_handle<> callback) noexcept {
        base_type::promise().set_callback(callback);
        return base_type::get_raw_handle();
    }
    auto await_resume() { return base_type::promise().get_value(); }
};
