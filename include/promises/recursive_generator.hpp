#pragma once
#include <promises/promise_base.hpp>
#include <suspend_maybe.hpp>
#include <unique_handle.hpp>

struct nothing_t {
    explicit nothing_t() = default;

    template <class Coro>
    operator Coro() const {
        co_return nothing_t();
    }
};

constexpr auto nothing = nothing_t();

namespace promise {
template <
    // Type output by generator
    class T>
struct recursive_generator_promise;

template <
    // Type output by generator
    class T>
struct recursive_generator_promise
    : promise_base< recursive_generator_promise<T>, no_return_void> {

    using base_type = promise_base<recursive_generator_promise, no_return_void>;
    using return_object = unique_handle<recursive_generator_promise>;

   public:
    return_object* sauce = nullptr;

   public:
    using base_type::yield_value;

    /* --- IMPORTANT --- */
    /*
        I believe that there is a bug in gcc where the compiler will construct
        the return object *after* the call to initial_suspend if get_return_object
        doesn't directly return the return object.

        In other words, if get_return_object returns a handle,
        then the compiler will construct unique_handle *after* the coroutine
        finally suspends. This results in a segfault in some cases.

        The workaround is to explicitly create a get_return_object that returns a unique_handle
        in classes for which the ordering of initial_suspend and get_return_object is important.
    */
    auto get_return_object() { return return_object{base_type::get_handle()}; }
    void set_return_object(return_object* sauce) { this->sauce = sauce; }

    void return_value(return_object new_generator) {
        sauce->assign_no_destroy(std::move(new_generator));
    }
    void return_value(nothing_t) {}

    suspend_maybe final_suspend() noexcept {
        // If the sauce is null, this coroutine has been detached
        // so await_ready should indicate true in order to clean up coroutine
        return suspend_maybe{sauce == nullptr};
    }
    // yielded value stored here
    T value;

    // Stores value in this->value, to be accessed by the caller via
    // coroutine_handle.promise().value
    constexpr auto yield_value(T value) noexcept(move_T_noexcept) {
        this->value = std::move(value);
        return std::suspend_always{};
    }

   private:
    // These are just used to get a reference to T in template expressions
    static T& mutable_T();
    static T&& moved_T();
    constexpr static bool move_T_noexcept = noexcept(mutable_T() = moved_T());
};
}
template <class T>
using recursive_generator = unique_handle<promise::recursive_generator_promise<T>>;
