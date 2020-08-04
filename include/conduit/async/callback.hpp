#pragma once
#include <conduit/async/jump.hpp>
#include <conduit/common.hpp>
#include <conduit/mixin/awaitable_parts.hpp>

namespace conduit::async {
namespace with_nullptr {
// Implements async::callback using a default state of nullptr
class callback : mixin::AwaitReady<false>, mixin::AwaitResume {
    std::coroutine_handle<> h = nullptr;
    static inline auto release(std::coroutine_handle<>& h) noexcept
        -> std::coroutine_handle<> {
        auto hold = h;
        h = nullptr;
        return hold;
    }

   public:
    using self = callback;
    constexpr callback() = default;
    constexpr callback(std::nullptr_t) noexcept : h(nullptr) {}
    constexpr callback(std::coroutine_handle<> h) noexcept : h(h) {}
    callback(const callback&) = delete;
    callback(callback&& source) noexcept : h(release(source.h)) {}
    // Releases ownership of the coroutine and produces an async::jump
    // with that coroutine
    [[nodiscard]] auto release() noexcept -> jump {
        return jump{release(h)};
    }
    void emplace(std::coroutine_handle<> handle) noexcept {
        h = handle;
    }

    ~callback() {
        if (h)
            h.destroy();
    }
    constexpr void swap(callback& other) noexcept {
        auto temp = other.h;
        other.h = h;
        h = temp;
    }
    using AwaitReady<false>::await_ready;
    using AwaitResume::await_resume;
    auto await_suspend(std::coroutine_handle<>) noexcept
        -> std::coroutine_handle<> {
        return h ? release(h) : std::noop_coroutine();
    }
};
} // namespace with_nullptr

namespace with_noop {
// Implements async::callback using a default state of std::noop_coroutine
class callback : mixin::AwaitReady<false>, mixin::AwaitResume {
    std::coroutine_handle<> h = std::noop_coroutine();
    // Bypasses M::sanitize, used by release() method
    constexpr callback(std::coroutine_handle<> h, int) noexcept : h(h) {}
    static inline auto release(std::coroutine_handle<>& h) noexcept
        -> std::coroutine_handle<> {
        auto copy = h;
        h = std::noop_coroutine();
        return copy;
    }

   public:
    using self = callback;
    callback() = default;
    callback(std::nullptr_t) noexcept : h(std::noop_coroutine()) {}
    callback(std::coroutine_handle<> h) noexcept
      : h(h ? h : std::noop_coroutine()) {}
    callback(const callback&) = delete;
    callback(callback&& source) noexcept : h(release(source.h)) {}

    [[nodiscard]] auto release() noexcept -> callback {
        return callback(release(h), 0);
    }
    [[nodiscard]] auto release_jump() noexcept -> jump { return jump{release(h)}; }
    auto operator=(callback source) noexcept -> callback& {
        swap(source);
        return *this;
    }
    auto operator=(std::nullptr_t) -> self& {
        release(h).destroy();
        return *this;
    }
    ~callback() { h.destroy(); }
    void swap(callback& other) noexcept {
        auto temp = other.h;
        other.h = h;
        h = temp;
    }
    using AwaitReady<false>::await_ready;
    using AwaitResume::await_resume;
    auto await_suspend(std::coroutine_handle<>) noexcept
        -> std::coroutine_handle<> {
        return release(h);
    }
};
} // namespace with_noop

// Holds ownership of a coroutine handle until it's co_await`d on,
// at which point it releases ownership of the coroutine, and the
// coroutine is resumed
using callback = with_nullptr::callback;
} // namespace conduit::async
