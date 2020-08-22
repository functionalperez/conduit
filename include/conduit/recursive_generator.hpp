#pragma once
#include <conduit/async/await_if.hpp>
#include <conduit/iterator.hpp>
#include <conduit/mixin/promise_parts.hpp>
#include <conduit/unique_handle.hpp>

namespace conduit::promise {
template <
    // Type output by generator
    class T>
struct recursive_generator;

template <
    // Type output by generator
    class T>
struct recursive_generator : mixin::InitialSuspend<false>,
                             mixin::UnhandledException<> {
    using return_object = unique_handle<recursive_generator>;
    using handle_type = std::coroutine_handle<recursive_generator>;
    return_object* sauce = nullptr;
    T const* pointer = nullptr;

   public:
    /* --- IMPORTANT --- */
    /*
        I believe that there is a bug in gcc where the compiler will construct
        the return object *after* the call to initial_suspend if
       get_return_object doesn't directly return the return object.

        In other words, if get_return_object returns a handle,
        then the compiler will construct unique_handle *after* the coroutine
        finally suspends. This results in a segfault in some cases.

        The workaround is to explicitly create a get_return_object that returns
       a unique_handle in classes for which the ordering of initial_suspend and
       get_return_object is important.
    */
    auto get_return_object() {
        return return_object{handle_type::from_promise(*this)};
    }
    void set_return_object(return_object* sauce) { this->sauce = sauce; }

    void return_value(return_object new_generator) {
        sauce->assign_no_destroy(std::move(new_generator));
    }
    void return_value(nothing_t) {}

    async::continue_if final_suspend() noexcept {
        // If the sauce is null, this coroutine has been coroutine
        // so await_ready should indicate true in order to clean up coroutine
        return async::continue_if{sauce == nullptr};
    }

    // Stores value in this->value, to be accessed by the caller via
    // coroutine_handle.promise().value
    constexpr auto yield_value(T const& v) noexcept {
        pointer = &v;
        return std::suspend_always{};
    }
    constexpr T const& value() noexcept { return *pointer; }
};
} // namespace conduit::promise

namespace conduit {
template <class T>
using recursive_generator = unique_handle<promise::recursive_generator<T>>;

template <class T>
auto begin(recursive_generator<T>& g)
    -> coro_iterator<recursive_generator<T>&> {
    return {g};
}
template <class T>
auto end(recursive_generator<T>& g) -> coro_iterator<recursive_generator<T>&> {
    return {g};
}
} // namespace conduit