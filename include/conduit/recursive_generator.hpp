#pragma once
#include <conduit/async/await_if.hpp>
#include <conduit/mixin/promise_parts.hpp>
#include <conduit/util/iterator.hpp>
#include <conduit/util/unique_handle.hpp>

namespace conduit {
template <class T>
struct recursive_generator;
}
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
    using return_object = conduit::recursive_generator<T>;
    using handle_type = std::coroutine_handle<recursive_generator>;
    unique_handle<recursive_generator>* sauce = nullptr;
    T const* pointer = nullptr;

   public:
    auto get_return_object() {
        return return_object{handle_type::from_promise(*this)};
    }
    void set_return_object(unique_handle<recursive_generator>* sauce) { this->sauce = sauce; }

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
    constexpr T const& get_value() noexcept { return *pointer; }
};
} // namespace conduit::promise

namespace conduit {
template <class T>
struct recursive_generator : unique_handle<promise::recursive_generator<T>> {
    using promise_type = promise::recursive_generator<T>;
    using unique_handle<promise_type>::unique_handle;
};

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
