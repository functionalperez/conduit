#pragma once
#include <conduit/async/coro.hpp>
#include <conduit/mixin/awaitable_parts.hpp>
#include <conduit/mixin/promise_parts.hpp>
#include <conduit/util/optional_ref.hpp>
#include <stdexcept>

#include <optional>

namespace conduit::promise {
template <class T>
class source
  : mixin::YieldValue<T>
  , public mixin::ExceptionHandler
  , public mixin::InitialSuspend<true>
  , public mixin::ReturnVoid {
    std::coroutine_handle<> callback = nullptr;

   public:
    using coroutine_handle = std::coroutine_handle<source>;
    using YieldValue = mixin::YieldValue<T>;
    using value_type = typename YieldValue::value_type;
    using reference_type = typename YieldValue::reference_type;
    using pointer_type = typename YieldValue::pointer_type;
    using ExceptionHandler::rethrow_if_exception;
    using YieldValue::get_pointer;
    using YieldValue::has_value;
    using YieldValue::value;
    bool undergoing_destruction() noexcept {
        return callback == coroutine_handle::from_promise(*this);
    }
    void clear_callback() noexcept { callback = nullptr; }
    void set_callback(std::coroutine_handle<> handle) noexcept {
        callback = handle;
    }

    async::jump yield_value(reference_type value) noexcept {
        YieldValue::yield_value(value);
        return async::jump {callback};
    }
    async::jump final_suspend() noexcept {
        return async::jump { callback };
    }

    auto get_return_object() noexcept {
        return coroutine_handle::from_promise(*this);
    }

    ~source() {
        if (callback) {
            auto handle = callback;
            callback = coroutine_handle::from_promise(*this);
            handle.destroy();
        }
    }
};
// template <>
// struct source<void>
//   : mixin::HasOwnerAndCallback
//   , mixin::GetReturnObject<source<void>>
//   , mixin::UnhandledException<source<void>>
//   , mixin::ReturnVoid {
//    public:
//     auto yield_value(tags::nothing_t) noexcept { return callback.release(); }
//     auto yield_value(int) noexcept { return callback.release(); }
// };
} // namespace conduit::promise

namespace conduit {
template <class T>
struct await_source {
    using promise_type = promise::source<T>;
    using coroutine_handle = std::coroutine_handle<promise::source<T>>;
    using reference_type = typename promise_type::reference_type;
    using return_type = optional_ref<std::remove_reference_t<reference_type>>;
    coroutine_handle handle;

    constexpr bool await_ready() const noexcept { return false; }
    auto await_suspend(std::coroutine_handle<> callback) const noexcept
        -> coroutine_handle {
        handle.promise().set_callback(callback);
        return handle;
    }
    return_type await_resume() {
        if (handle.done()) {
            handle.promise().rethrow_if_exception();
            return {nullptr};
        } else {
            return {&handle.promise().value()};
        }
    }
};
template <class T>
class source : std::coroutine_handle<promise::source<T>> {
    using super = std::coroutine_handle<promise::source<T>>;
    using super::destroy;
    using super::promise;

   public:
    using promise_type = promise::source<T>;
    using coroutine_handle = std::coroutine_handle<promise_type>;
    using reference_type = typename promise_type::reference_type;
    using optional_ref_type =
        optional_ref<std::remove_reference_t<reference_type>>;
    using super::operator bool;

    source() = default;
    source(source const&) = delete;
    source(source&& other) noexcept
      : super(other) {
        other = nullptr;
    }
    source(coroutine_handle h) noexcept
      : super(h) {}
    source& operator=(source s) noexcept {
        swap(s);
        return *this;
    }

    auto operator co_await() & { return await_source<T> {*this}; }
    ~source() {
        if (*this) {
            auto& p = promise();
            if (!p.undergoing_destruction()) {
                p.clear_callback();
                destroy();
            }
        }
    }
    void swap(source& other) noexcept {
        coroutine_handle hold = other;
        other.super::operator=(*this);
        super::operator=(hold);
    }
};
} // namespace conduit
