#pragma once
#include <conduit/async/jump.hpp>
#include <conduit/mixin/awaitable_parts.hpp>

namespace conduit::async {
// Holds ownership of a coroutine handle until it's co_await`d on,
// at which point it releases ownership of the coroutine, and the
// coroutine is resumed
class callback {
    std::coroutine_handle<> h = nullptr;
    static inline auto release(std::coroutine_handle<>& h) noexcept
        -> std::coroutine_handle<> {
        auto hold = h;
        h = nullptr;
        return hold;
    }

   public:
    using self = callback;
    callback() = default;
    callback(std::nullptr_t) noexcept : h(nullptr) {}
    callback(std::coroutine_handle<> h) noexcept : h(h) {}
    callback(const callback&) = delete;
    callback(callback&& source) noexcept : h(release(source.h)) {}
    // Releases ownership of the coroutine and produces an async::jump
    // with that coroutine
    [[nodiscard]] inline auto release() noexcept -> jump { return jump{release(h)}; }
    inline void emplace(std::coroutine_handle<> handle) noexcept {
        h = handle;
    }

    inline void swap(callback& other) noexcept {
        auto temp = other.h;
        other.h = h;
        h = temp;
    }
    inline void resume() {
        release(h).resume();
    }
    inline void operator()() {
        release(h).resume();
    }
    ~callback() {
        if (h)
            h.destroy();
    }
};
} // namespace conduit::async
