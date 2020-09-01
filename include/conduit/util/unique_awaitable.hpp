#include <utility>
#include <conduit/util/stdlib_coroutine.hpp>
#include <conduit/async/callback.hpp>

namespace conduit {
template <class Awaitable>
struct unique_awaitable {
    Awaitable* pointer = nullptr;

    unique_awaitable() = default;
    unique_awaitable(Awaitable* pointer) noexcept : pointer(pointer) {}
    unique_awaitable(unique_awaitable const&) = default;
    unique_awaitable(unique_awaitable&& other) noexcept
      : pointer(other.pointer) {
        other.pointer = nullptr;
    }

    unique_awaitable& operator=(unique_awaitable other) {
        std::swap(pointer, other.pointer);
        return *this;
    }
    async::callback release() {
        std::coroutine_handle<> caller = pointer->caller;
        pointer = nullptr;
        return async::callback{caller};
    }
    Awaitable* operator->() const { return pointer; }
    ~unique_awaitable() {
        if (pointer) {
            pointer->caller.destroy();
        }
    }
};
} // namespace conduit