#pragma once
#include <conduit/stdlib_coroutine.hpp>
#include <iterator>

namespace conduit {
struct coro_sentinal {};

template <class Handle>
struct coro_iterator {
    Handle coro;

    // Resumes the coroutine; returns reference to this iterator
    void operator++() { coro.resume(); }
    // Resumes the coroutine
    void operator++(int) { coro.resume(); }

    // Calls deref on the coroutine handle and returns the result
    // By default, this returns coro.promise().current_value;
    decltype(auto) operator*() noexcept { return coro.promise().value(); }

    // Returns true iff the coroutine is done
    bool operator==(coro_sentinal) const noexcept { return coro.done(); }
    // Returns true iff the coroutine is not done
    bool operator!=(coro_sentinal) const noexcept { return !coro.done(); }

    bool operator==(coro_iterator) const noexcept { return coro.done(); }
    // Returns true iff the coroutine is not done
    bool operator!=(coro_iterator) const noexcept { return !coro.done(); }

    using iterator_category = std::input_iterator_tag;
    using value_type =
        std::remove_reference_t<decltype(coro.promise().value())>;
    using pointer = value_type const*;
    using reference = value_type const&;
    using difference_type = std::ptrdiff_t;
};

} // namespace conduit
