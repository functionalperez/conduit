#pragma once
#include <awaitable_callback.hpp>
#include <promise_base.hpp>
#include <unique_handle.hpp>

template <class Promise>
struct awaitable_task : unique_handle<Promise> {
    using base_type = unique_handle<Promise>;
    using base_type::base_type;

    bool await_ready() noexcept { return base_type::done(); }
    auto await_suspend(std::coroutine_handle<> callback) noexcept {
        base_type::promise().set_callback(callback);
        return base_type::get_raw_handle();
    }
    auto await_resume() { return base_type::promise().get_value(); }
};

template <class ReturnValue>
struct task_promise : promise_base<task_promise<ReturnValue>, awaitable_task, true, false> {
    std::coroutine_handle<> callback = std::noop_coroutine();
    ReturnValue returned_value = {};

    auto final_suspend() noexcept { return awaitable_callback{callback}; }
    void set_callback(std::coroutine_handle<> callback) noexcept { this->callback = callback; }
    void return_value(ReturnValue value) { returned_value = std::move(value); }
    ReturnValue get_value() { return std::move(returned_value); }
};

template <class T>
using future = awaitable_task<task_promise<T>>;
