#pragma once
#include <conduit/mixin/promise_parts.hpp>
#include <conduit/util/iterator.hpp>
#include <conduit/util/unique_handle.hpp>
#include <memory>

namespace conduit::promise {
template <class T>
struct generator
  : mixin::GetReturnObject<generator<T>>
  , mixin::InitialSuspend<false>
  , mixin::FinalSuspend<true>
  , mixin::ReturnVoid {
   public:
    constexpr static bool is_reference = std::is_reference_v<T>;
    using value_type = std::remove_reference_t<T>;
    using pointer = value_type*;
    using reference_type = value_type&;
    // Stores value in this->value, to be accessed by the caller via
    // coroutine_handle.promise().value
    constexpr auto yield_value(T& v) noexcept {
        value_pointer = std::addressof(v);
        return std::suspend_always {};
    }
    constexpr auto yield_value(T&& v) noexcept {
        value_pointer = std::addressof(v);
        return std::suspend_always {};
    }

    void unhandled_exception() noexcept {
        exception_pointer = std::current_exception();
    }

    reference_type get_value() const noexcept { return *value_pointer; }

    void rethrow_if_exception() {
        if (exception_pointer)
            std::rethrow_exception(exception_pointer);
    }

   private:
    std::exception_ptr exception_pointer = nullptr;
    pointer value_pointer;
};
} // namespace conduit::promise

namespace conduit {
template <class T>
struct generator : unique_handle<promise::generator<T>> {
    using promise_type = promise::generator<T>;
    using unique_handle<promise::generator<T>>::unique_handle;
    using unique_handle<promise::generator<T>>::promise;
};

template <class T>
auto begin(generator<T>& g)
    -> coro_iterator<std::coroutine_handle<promise::generator<T>>> {
    if (g.done()) {
        g.promise().rethrow_if_exception();
    }
    return {g.get()};
}
template <class T>
auto end(generator<T>& g)
    -> coro_iterator<std::coroutine_handle<promise::generator<T>>> {
    return {g.get()};
}

template <class T>
bool operator>>(generator<T>& g, T& value) {
    if (g.done())
        return false;
    value = g->value;
    g.resume();
    return true;
}
} // namespace conduit
