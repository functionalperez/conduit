#pragma once
#include <common.hpp>

namespace mini {
template <class T>
struct generator {
    struct promise_type {
        T current_value;
        auto yield_value(T value) {
            current_value = value;
            return std::suspend_always{};
        }
        generator get_return_object() {
            return {std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        auto initial_suspend() noexcept { return std::suspend_always(); }
        auto final_suspend() noexcept { return std::suspend_always(); }
        void unhandled_exception() noexcept { std::terminate(); }
        void return_void() noexcept {}
    };
    std::coroutine_handle<promise_type> handle = nullptr;

    void destroy() {
        if(handle) {
            handle.destroy();
            handle = nullptr; 
        }
    }
    ~generator() {
        destroy(); 
    }
};
}