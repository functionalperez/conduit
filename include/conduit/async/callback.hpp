#pragma once
#include <conduit/async/jump.hpp>
#include <conduit/common.hpp>
#include <conduit/mixin/awaitable_parts.hpp>

namespace conduit::async {
// Holds ownership of a coroutine handle until it's co_await`d on,
// at which point it releases ownership of the coroutine, and the
// coroutine is resumed
class callback : mixin::AwaitReady<false>, mixin::AwaitResume {
    std::coroutine_handle<> h = nullptr;
    static auto release(std::coroutine_handle<>& h) noexcept
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
    [[nodiscard]] // async::jump should not be discarded 
    auto release() noexcept -> jump {
        return jump{release(h)};
    }
    constexpr void emplace(std::coroutine_handle<> handle) noexcept {
        h = handle;
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
    ~callback() {
        if (h)
            h.destroy();
    }
};
} // namespace conduit::async
