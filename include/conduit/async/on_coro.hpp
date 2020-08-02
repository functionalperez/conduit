#pragma once
#include <conduit/unique_handle.hpp>

namespace conduit::async {
template <class Promise, bool assume_ownership>
struct on_coro;

template<class Promise>
struct on_coro<Promise, false> {
    unique_handle<Promise>& owner;
    std::coroutine_handle<Promise> handle = nullptr;

    constexpr bool await_ready() noexcept {
        handle = owner.release();
        return handle.done();
    }
    constexpr auto await_suspend(std::coroutine_handle<> callback) noexcept
        -> std::coroutine_handle<> {
        handle.promise().set_callback(callback);
        return handle;
    }
    // Returns promise().get_value() if get_value() exists
    constexpr auto await_resume() {
        owner.assign_no_destroy(handle);
        if constexpr (value_producing_promise<Promise&>) {
            return handle.promise().get_value();
        }
    }
};
template <class Promise>
struct on_coro<Promise, true> {
    unique_handle<Promise> owner;
    std::coroutine_handle<Promise> handle = nullptr;

    constexpr bool await_ready() noexcept {
        handle = owner.release();
        return handle.done();
    }
    constexpr auto await_suspend(std::coroutine_handle<> callback) noexcept
        -> std::coroutine_handle<> {
        handle.promise().set_callback(callback);
        return handle;
    }
    // Returns std::move(promise()).get_value() if get_value() exists
    constexpr auto await_resume() {
        owner.assign_no_destroy(handle);
        if constexpr (value_producing_promise<Promise&&>) {
            return std::move(handle.promise()).get_value();
        }
    }
};
template <class Promise>
on_coro(unique_handle<Promise>&) -> on_coro<Promise, false>;
template <class Promise>
on_coro(unique_handle<Promise> &&) -> on_coro<Promise, true>;
} // namespace conduit::async
