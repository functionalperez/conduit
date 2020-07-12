#pragma once
#include <conduit/common.hpp>
#include <iostream>

namespace mini {
template <class T>
struct task {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    struct promise_type {
        auto initial_suspend() { return std::suspend_always{}; }
        auto final_suspend() { return std::suspend_always{}; }
        auto get_return_object() {
            return task{handle_type::from_promise(*this)};
        }
        void unhandled_exception() { std::terminate(); }
        void return_value(T value) {
            std::cout << "Obtained " << value << '\n';
        }
    };

    std::coroutine_handle<promise_type> handle;
};
} // namespace mini
