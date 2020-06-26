#pragma once
#include <common.hpp>
#include <unique_handle.hpp>

// This is an incomplete promise_base
template <bool SuspendInitially>
struct promise_base_base {
    constexpr auto initial_suspend() noexcept {
        if constexpr (SuspendInitially)
            return std::suspend_always{};
        else
            return std::suspend_never{};
    }
    constexpr auto final_suspend() noexcept { return std::suspend_always(); }
    [[noreturn]] void unhandled_exception() noexcept { std::terminate(); }
    constexpr void return_void() noexcept {}
};

template <
    // This is the class that derives from promise_base.
    class Derived,
    // This is the ReturnObject template that's instantiated with Derived
    template <class> class ReturnObject = unique_handle,
    // And this determines whether or not initial_suspend returns
    // suspend_always or suspend_never
    bool SuspendInitially = true>
struct promise_base : promise_base_base<SuspendInitially> {
    using handle = std::coroutine_handle<Derived>;
    using return_object = ReturnObject<Derived>;

    // If there's an allocation failure, returns a null coroutine handle
    static auto get_return_object_on_allocation_failure() noexcept {
        return return_object{nullptr};
    }

    // Used by the compiler to produce the return_object when starting the coroutine
    auto get_return_object() noexcept { return return_object{handle::from_promise(get_promise())}; }

   private:
    // Obtains the promise object from this
    Derived& get_promise() { return static_cast<Derived&>(*this); }
};
