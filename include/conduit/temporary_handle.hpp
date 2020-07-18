#pragma once

#include <conduit/common.hpp>
#include <utility>

namespace conduit {
class temporary_handle {
    std::coroutine_handle<> handle = nullptr;

   public:
    temporary_handle() = default;
    temporary_handle(std::coroutine_handle<> handle) : handle(handle) {}
    temporary_handle(const temporary_handle&) = delete;
    temporary_handle(temporary_handle&& source) : handle(std::exchange(source.handle, nullptr)) {}

    auto operator=(temporary_handle h) -> temporary_handle& {
        swap(h);
        return *this;
    }
    auto operator=(std::nullptr_t) -> temporary_handle& {
        if (handle) {
            std::exchange(handle, nullptr).destroy();
        }
        return *this;
    }

    void resume() { std::exchange(handle, nullptr).resume(); }
    std::coroutine_handle<> release() {
        return std::exchange(handle, nullptr);
    }
    ~temporary_handle() {
        if (handle) {
            handle.destroy();
        }
    }
    void swap(temporary_handle& other) {
        auto temp = other.handle;
        other.handle = handle;
        handle = temp;
    }
};
} // namespace conduit
