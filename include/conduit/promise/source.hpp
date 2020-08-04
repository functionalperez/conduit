#pragma once
#include <conduit/mixin/promise_parts.hpp>
#include <conduit/optional_ref.hpp>
#include <conduit/unique_handle.hpp>

#include <optional>

namespace conduit::promise {
template <class ReturnValue>
struct source : mixin::GetReturnObject<source<ReturnValue>>,
                mixin::PromiseWithCallback,
                mixin::UnhandledException<true> {
   private:
    ReturnValue const* pointer;

   public:
    async::jump yield_value(ReturnValue const& value) {
        pointer = &value;
        return callback.release();
    }
    void return_void() { pointer = nullptr; }

    constexpr bool await_ready() noexcept { return false; }
    auto await_suspend(std::coroutine_handle<> handle) noexcept
        -> std::coroutine_handle<> {
        callback.emplace(handle);
        return std::coroutine_handle<source>::from_promise(*this);
    }
    constexpr optional_ref<ReturnValue const> await_resume() noexcept {
        return optional_ref{pointer};
    }
    // optional_ref<ReturnValue const> get_value() & { return
    // optional_ref(pointer); }
};
} // namespace conduit::promise
