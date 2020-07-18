#pragma once
#include <conduit/unique_handle.hpp>

namespace conduit::async {
template <class Promise, bool assume_ownership = false>
class on_coro {
    unique_handle<Promise>& owner;
    std::coroutine_handle<Promise> handle = nullptr;

   public:
    on_coro(unique_handle<Promise>& owner) : owner(owner) {}
    constexpr bool await_ready() noexcept {
        handle = owner.release();
        return handle.done();
    }
    constexpr auto await_suspend(std::coroutine_handle<> callback) noexcept {
        handle.promise().set_callback(callback);
        return handle;
    }
    // Returns promise().get_value()
    constexpr auto await_resume() const {
        owner.assign_no_destroy(handle);
        return handle.promise().get_value();
    }
};
template <class Promise>
class on_coro<Promise, true> {
   public:
    unique_handle<Promise> owner;
    std::coroutine_handle<Promise> handle = nullptr;

    constexpr bool await_ready() noexcept {
        handle = owner.release();
        return handle.done();
    }
    constexpr auto await_suspend(std::coroutine_handle<> callback) noexcept {
        handle.promise().set_callback(callback);
        return handle;
    }
    // Returns std::move(promise()).get_value()
    constexpr auto await_resume() {
        owner.assign_no_destroy(handle);
        return std::move(handle.promise()).get_value();
    }
};
template <class Promise>
on_coro(unique_handle<Promise>&) -> on_coro<Promise, false>;
template <class Promise>
on_coro(unique_handle<Promise> &&) -> on_coro<Promise, true>;
} // namespace conduit::async
