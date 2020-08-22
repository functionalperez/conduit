#pragma once
#include <conduit/async/on_coro.hpp>
#include <conduit/util/concepts.hpp>
#include <cstddef>

namespace conduit::async {
template <co_promise Promise>
class coro {
    std::coroutine_handle<Promise> handle = nullptr;
    std::coroutine_handle<Promise> release() {
        auto hold = handle;
        handle = nullptr;
        return hold;
    }

   public:
    using promise_type = Promise;
    using handle_type = std::coroutine_handle<Promise>;

    constexpr coro() = default;
    coro(coro const&) = delete;
    constexpr coro(std::nullptr_t) noexcept : coro() {}
    coro(std::coroutine_handle<Promise> h) noexcept : handle(h) {
        if (handle) {
            handle.promise().set_owner(&handle);
        }
    }
    constexpr coro(coro&& source) noexcept : handle(source.release()) {
        if (handle) {
            handle.promise().set_owner(&handle);
        }
    }

    constexpr coro& operator=(coro c) noexcept {
        swap(c);
        return *this;
    }

    constexpr void swap(coro& other) noexcept {
        std::swap(handle, other.handle);
        if (handle)
            handle.promise().set_owner(&handle);
        if (other.handle)
            handle.promise().set_owner(&other.handle);
    }

    // awaits on the given coroutine
    auto operator co_await() & noexcept {
        return on_coro<Promise, false>{handle};
    }

#if defined(__GNUC__) && !defined(__clang__)
    // co_awaiting directly on a newly created coroutine causes a memory leak in
    // gcc
    auto operator co_await() && noexcept = delete;
#else
    // awaits on the given coroutine
    auto operator co_await() && noexcept {
        return on_coro<Promise, true>{handle};
    }
#endif
    bool done() const { return handle.done(); }

    ~coro() {
        if (handle) {
            handle.destroy();
        }
    }
};
} // namespace conduit::async