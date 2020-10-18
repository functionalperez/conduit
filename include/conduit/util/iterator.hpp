#pragma once
#include <conduit/util/concepts.hpp>
#include <conduit/util/stdlib_coroutine.hpp>
#include <iterator>

namespace conduit {
struct coro_sentinel {};

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
        return coro.promise().get_value();
    }

    // Returns true iff the coroutine is done
    bool operator==(coro_sentinel) const noexcept { return coro.done(); }
    // Returns true iff the coroutine is not done
    bool operator!=(coro_sentinel) const noexcept { return !coro.done(); }

    bool operator==(coro_iterator const&) const noexcept { return coro.done(); }
    // Returns true iff the coroutine is not done
    bool operator!=(coro_iterator const&) const noexcept {
        return !coro.done();
    }

    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;
    using value_type = std::decay_t<decltype(*std::declval<coro_iterator>())>;
    using pointer = value_type const*;
    using reference = value_type const&;
};

template <typename Promise>
class coro_iterator<std::coroutine_handle<Promise>> {
    using coroutine_handle = std::coroutine_handle<Promise>;

   public:
    using iterator_category = std::input_iterator_tag;
    // What type should we use for counting elements of a potentially infinite
    // sequence?
    using difference_type = std::ptrdiff_t;
    using value_type = typename Promise::value_type;
    using reference = typename Promise::reference_type;
    using pointer = typename Promise::pointer_type;

    // Iterator needs to be default-constructible to satisfy the Range concept.
    coro_iterator() = default;
    coro_iterator(const coro_iterator&) = default;
    coro_iterator(coroutine_handle coroutine) noexcept
      : coro(coroutine) {}

    bool operator==(const coro_iterator&) const noexcept {
        return !coro || coro.done();
    }
    bool operator!=(const coro_iterator&) const noexcept {
        return coro && !coro.done();
    }

    bool operator==(coro_sentinel) const noexcept {
        return !coro || coro.done();
    }

    bool operator!=(coro_sentinel) noexcept { return coro && !coro.done(); }

    friend bool operator==(coro_sentinel s, const coro_iterator& it) noexcept {
        return it == s;
    }

    friend bool operator!=(coro_sentinel s, const coro_iterator& it) noexcept {
        return it != s;
    }

    coro_iterator& operator++() {
        coro.resume();
        if (coro.done()) {
            coro.promise().rethrow_if_exception();
        }

        return *this;
    }

    // Need to provide post-increment operator to implement the 'Range' concept.
    void operator++(int) { (void)operator++(); }

    reference operator*() const noexcept { return coro.promise().value(); }

    pointer operator->() const noexcept { return std::addressof(coro.promise().value()); }

   private:
    coroutine_handle coro;
};
} // namespace conduit
