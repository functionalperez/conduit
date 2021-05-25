#pragma once
#include <conduit/async/callback.hpp>
#include <conduit/async/immediate_value.hpp>
#include <conduit/mem/allocator.hpp>
#include <conduit/mixin/awaitable_parts.hpp>
#include <conduit/util/tag_types.hpp>
#include <exception>

namespace conduit::mixin {
enum suspend : bool { always = true, never = false };

#if CONDUIT_USE_GCC_EXCEPTION_WORKAROUND
namespace detail {
using remuse_coro_t = void (*)();
using destroy_coro_t = void (*)();
struct frame_header_t {
    remuse_coro_t resume_coro;
    destroy_coro_t destroy_coro;
};
} // namespace detail
template <bool suspend>
struct InitialSuspend {
    // If CONDUIT_USE_GCC_EXCEPTION_WORKAROUND is defined, then we need to keep
    // track of this value in order to destroy the frame manually. This value is
    // recorded inside initial_suspend_t
    detail::destroy_coro_t destroy_coro = nullptr;

    struct initial_suspend_t {
        detail::destroy_coro_t& destroy_coro_ref;

        inline constexpr bool await_ready() { return false; }
        inline bool await_suspend(std::coroutine_handle<> h) {
            destroy_coro_ref =
                ((detail::frame_header_t*)h.address())->destroy_coro;
            return suspend; // The coroutine is resumed if suspend is false
        }
        inline constexpr void await_resume() noexcept {}
    };

    inline constexpr auto initial_suspend() noexcept {
        return initial_suspend_t {destroy_coro};
    }
};
#else
template <bool suspend>
struct InitialSuspend {
    inline constexpr auto initial_suspend() noexcept {
        if constexpr (suspend) {
            return std::suspend_always {};
        } else {
            return std::suspend_never {};
        }
    }
};
#endif
template <bool suspend>
struct FinalSuspend {
    inline constexpr auto final_suspend() noexcept {
        if constexpr (suspend) {
            return std::suspend_always {};
        } else {
            return std::suspend_never {};
        }
    }
};
struct ReturnVoid {
    inline constexpr void return_void() noexcept {}
};

template <class DerivedPromise>
struct UnhandledException {
    void unhandled_exception() {
        // NB: for some reason, GCC doesn't destroy the coroutine frame if
        // there's an exception raised inside the coroutine. As a result, if
        // we're on GCC, we need to destroy it manually.

#ifdef CONDUIT_USE_GCC_EXCEPTION_WORKAROUND
        DerivedPromise& promise = static_cast<DerivedPromise&>(*this);
        auto coro_frame = static_cast<detail::frame_header_t*>(
            std::coroutine_handle<DerivedPromise>::from_promise(promise)
                .address());
        coro_frame->destroy_coro = promise.destroy_coro;
        std::coroutine_handle<>::from_address(coro_frame).destroy();
#endif

        std::rethrow_exception(std::current_exception());
    }
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
    inline auto yield_value(tags::get_promise_t) noexcept {
        return async::immediate_value {static_cast<Promise*>(this)};
    }

    inline auto yield_value(tags::get_handle_t) noexcept {
        return async::immediate_value {get_handle()};
    }

    inline handle_type get_handle() noexcept {
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

template <class Alloc>
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

class HasOwnerAndCallback : public mixin::InitialSuspend<true> {
   protected:
    std::coroutine_handle<>* owner = nullptr;
    async::callback callback;

   public:
    template <class T>
    inline void set_owner(std::coroutine_handle<T>* owner) noexcept {
        this->owner = reinterpret_cast<std::coroutine_handle<>*>(owner);
    }
    inline void set_callback(std::coroutine_handle<> handle) noexcept {
        callback.emplace(handle);
    }

    inline auto final_suspend() noexcept { return callback.release(); }

    ~HasOwnerAndCallback() noexcept {
        if (owner) {
            *owner = nullptr;
        }
    }
};

class ExceptionHandler {
    std::exception_ptr exception_ptr;
    constexpr static bool unhandled_noexcept =
        std::is_nothrow_copy_assignable_v<std::exception_ptr>;

   public:
    void unhandled_exception() noexcept(unhandled_noexcept) {
        exception_ptr = std::current_exception();
    }
    void rethrow_if_exception() const {
        if (exception_ptr) {
            std::rethrow_exception(exception_ptr);
        }
    }
    void clear_and_rethrow_if_exception() {
        if (exception_ptr) {
            auto hold = exception_ptr;
            exception_ptr = nullptr;
            std::rethrow_exception(hold);
        }
    }
};

template <class T>
class YieldValue {
   public:
    using value_type = std::remove_reference_t<T>;
    using reference_type =
        std::conditional_t<std::is_reference_v<T>, T, T const&>;
    using pointer_type = std::remove_reference_t<reference_type>*;

   private:
    pointer_type value_ptr = nullptr;

   protected:
    void clear() noexcept { value_ptr = nullptr; }

   public:
    constexpr bool has_value() const noexcept { return value_ptr; }
    constexpr auto yield_value(reference_type value) noexcept
        -> std::suspend_always {
        value_ptr = std::addressof(value);
        return {};
    }
    constexpr auto get_pointer() const noexcept -> pointer_type {
        return value_ptr;
    }
    constexpr auto value() const noexcept -> reference_type {
        return *value_ptr;
    }
};

// Protects incorrect co_await operations by deleting await_transform
struct DisableCoAwait {
    template <typename U>
    std::suspend_never await_transform(U&& value) = delete;
};
} // namespace conduit::mixin
