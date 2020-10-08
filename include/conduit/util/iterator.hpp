#pragma once
#include <conduit/util/concepts.hpp>
#include <conduit/util/stdlib_coroutine.hpp>
#include <iterator>

namespace conduit {
struct coro_sentinal {};

template <class Handle>
struct coro_iterator {
    using promise_type =
        std::decay_t<decltype(std::declval<Handle>().promise())>;
    Handle coro;

    // Resumes the coroutine; returns reference to this iterator
    void operator++() {
        coro.resume();
        if (coro.done()) {
            coro.promise().rethrow_if_exception();
        }
    }
    // Resumes the coroutine
    void operator++(int) {
        coro.resume();
        if (coro.done()) {
            coro.promise().rethrow_if_exception();
        }
    }

    // Calls deref on the coroutine handle and returns the result
    // By default, this returns coro.promise().current_value;
    auto const& operator*() const noexcept {
        if constexpr (has_value_member<promise_type>)
            return coro.promise().value;
        else {
            return coro.promise().get_value();
        }
    }

    // Returns true iff the coroutine is done
    bool operator==(coro_sentinal) const noexcept { return coro.done(); }
    // Returns true iff the coroutine is not done
    bool operator!=(coro_sentinal) const noexcept { return !coro.done(); }

    bool operator==(coro_iterator) const noexcept { return coro.done(); }
    // Returns true iff the coroutine is not done
    bool operator!=(coro_iterator) const noexcept { return !coro.done(); }

    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;
    using value_type = std::decay_t<decltype(*std::declval<coro_iterator>())>;
    using pointer = value_type const*;
    using reference = value_type const&;
};
} // namespace conduit
