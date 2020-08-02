#pragma once
#include <conduit/async/immediate_value.hpp>
#include <conduit/async/callback.hpp>
#include <conduit/common.hpp>
#include <conduit/mem/allocator.hpp>
#include <conduit/mixin/awaitable_parts.hpp>

namespace conduit::mixin {
enum suspend : bool { always = true, never = false };

template <bool suspend>
struct InitialSuspend {
    constexpr auto initial_suspend() noexcept {
        if constexpr (suspend) {
            return std::suspend_always{};
        } else {
            return std::suspend_never{};
        }
    }
};
template <bool suspend>
struct FinalSuspend {
    constexpr auto final_suspend() noexcept {
        if constexpr (suspend) {
            return std::suspend_always{};
        } else {
            return std::suspend_never{};
        }
    }
};
struct ReturnVoid {
    constexpr void return_void() noexcept {}
};
template <bool IsNoexcept = true>
struct UnhandledException {
    [[noreturn]] void unhandled_exception() noexcept { std::terminate(); }
};
template <>
struct UnhandledException<false> {
    void unhandled_exception() noexcept {}
};
template <class Promise, bool IsNoexcept = true>
struct GetReturnObject;

template <class Promise>
struct GetReturnObject<Promise, false> {
    using handle_type = std::coroutine_handle<Promise>;
    // Used by the compiler to produce the return_object when starting the
    // coroutine
    handle_type get_return_object() noexcept { return get_handle(); }

    // Allows you access to the promise object from within a coroutine via
    // auto& promise = co_yield get_promise;
    // await_ready() always returns true
    auto yield_value(get_promise_t) noexcept {
        return async::immediate_value{static_cast<Promise*>(this)};
    }

    auto yield_value(get_handle_t) noexcept {
        return async::immediate_value{get_handle()};
    }

    handle_type get_handle() noexcept {
        return handle_type::from_promise(static_cast<Promise&>(*this));
    }
};

template <class Promise>
struct GetReturnObject<Promise, true> : GetReturnObject<Promise, false> {
    using super = GetReturnObject<Promise, false>;
    using super::get_handle;
    using super::get_return_object;
    using super::yield_value;
    using handle_type = typename super::handle_type;

    // If there's an allocation failure, returns a null coroutine handle
    static handle_type get_return_object_on_allocation_failure() noexcept {
        return nullptr;
    }
};

template <mem::allocator Alloc>
struct NewAndDelete {
    template <class... T>
    static void* operator new(size_t size, Alloc& alloc, T&&...) {
        return alloc.alloc(size);
    }
    template <class... T>
    static void* operator new(size_t size, Alloc* alloc, T&&...) {
        return alloc->alloc(size);
    }
    static void operator delete(void* pointer, size_t size) {
        std::decay_t<Alloc>::dealloc(pointer, size);
    }
};

struct PromiseWithCallback : InitialSuspend<true> {
    async::callback callback;
    auto final_suspend() noexcept { return callback.release_jump(); }
    void set_callback(std::coroutine_handle<> handle) { callback = handle; }
};
} // namespace conduit::mixin