#pragma once
#include <conduit/mixin/promise_parts.hpp>
#include <conduit/util/iterator.hpp>
#include <conduit/util/stdlib_coroutine.hpp>
#include <conduit/util/unique_handle.hpp>
#include <exception>
#include <type_traits>

namespace conduit::promise {
template <typename T>
class generator
  : public mixin::InitialSuspend<true>
  , public mixin::FinalSuspend<true>
  , public mixin::ExceptionHandler
  , public mixin::ReturnVoid
  , public mixin::YieldValue<T>
  , public mixin::DisableCoAwait {
   public:
    using coroutine_handle = std::coroutine_handle<generator>;
    using value_type = typename mixin::YieldValue<T>::value_type;
    using reference_type = typename mixin::YieldValue<T>::reference_type;
    using pointer_type = typename mixin::YieldValue<T>::pointer_type;

    generator() = default;

    auto get_return_object() noexcept {
        return coroutine_handle::from_promise(*this);
    }
};
} // namespace conduit::promise

namespace conduit {
template <typename T>
class [[nodiscard]] generator : unique_handle<promise::generator<T>> {
    using super = unique_handle<promise::generator<T>>;

   public:
    using promise_type = promise::generator<T>;
    using iterator = coro_iterator<std::coroutine_handle<promise_type>>;

    using super::done;
    using super::get;
    using super::promise;
    using super::super;
    generator() = default;
    generator(generator &&) = default;
    generator(const generator& other) = delete;

    generator& operator=(generator other) noexcept {
        super::swap(other);
        return *this;
    }

    iterator begin() const {
        auto coro = get();
        if (coro) {
            coro.resume();
            if (coro.done()) {
                coro.promise().rethrow_if_exception();
            }
        }

        return iterator {coro};
    }

    iterator end() const noexcept { return {}; }
};

template <class T>
auto begin(generator<T> const& gen) {
    return gen.begin();
}
template <class T>
auto end(generator<T> const& gen) {
    return gen.end();
}
} // namespace conduit
