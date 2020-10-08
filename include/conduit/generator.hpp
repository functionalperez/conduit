#pragma once
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Lewis Baker
// Licenced under MIT license. See LICENSE.txt for details.
///////////////////////////////////////////////////////////////////////////////

// Copyright 2017 Lewis Baker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <conduit/util/stdlib_coroutine.hpp>
#include <exception>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace conduit {
template <typename T>
class generator;

namespace promise {
template <typename T>
class generator {
   public:
    using value_type = std::remove_reference_t<T>;
    using reference_type =
        std::conditional_t<std::is_reference_v<T>, T, T const&>;
    using pointer_type = std::remove_reference_t<reference_type>*;

    generator() = default;

    auto get_return_object() noexcept {
        return std::coroutine_handle<generator<T>>::from_promise(
            *this);
    }

    constexpr std::suspend_always initial_suspend() const noexcept {
        return {};
    }
    constexpr std::suspend_always final_suspend() const noexcept { return {}; }

    std::suspend_always yield_value(reference_type value) noexcept {
        value_ptr = std::addressof(value);
        return {};
    }

    void unhandled_exception() noexcept(
        std::is_nothrow_copy_assignable_v<std::exception_ptr>) {
        exception_ptr = std::current_exception();
    }

    void return_void() noexcept {}

    reference_type value() const noexcept { return *value_ptr; }

    // Don't allow any use of 'co_await' inside the generator coroutine.
    template <typename U>
    std::suspend_never await_transform(U&& value) = delete;

    void rethrow_if_exception() {
        if (exception_ptr) {
            std::rethrow_exception(exception_ptr);
        }
    }

   private:
    pointer_type value_ptr;
    std::exception_ptr exception_ptr;
};
} // namespace promise

namespace detail {
struct generator_sentinel {};

template <typename T>
class generator_iterator {
    using coroutine_handle = std::coroutine_handle<promise::generator<T>>;

   public:
    using iterator_category = std::input_iterator_tag;
    // What type should we use for counting elements of a potentially infinite
    // sequence?
    using difference_type = std::ptrdiff_t;
    using value_type = typename promise::generator<T>::value_type;
    using reference = typename promise::generator<T>::reference_type;
    using pointer = typename promise::generator<T>::pointer_type;

    // Iterator needs to be default-constructible to satisfy the Range concept.
    generator_iterator() noexcept
      : m_coroutine(nullptr) {}

    explicit generator_iterator(coroutine_handle coroutine) noexcept
      : m_coroutine(coroutine) {}

    friend bool operator==(
        const generator_iterator& it, const generator_iterator&) noexcept {
        return !it.m_coroutine || it.m_coroutine.done();
    }
    friend bool operator!=(
        const generator_iterator& it, const generator_iterator&) noexcept {
        return it.m_coroutine && !it.m_coroutine.done();
    }

    friend bool
    operator==(const generator_iterator& it, generator_sentinel) noexcept {
        return !it.m_coroutine || it.m_coroutine.done();
    }

    friend bool
    operator!=(const generator_iterator& it, generator_sentinel s) noexcept {
        return !(it == s);
    }

    friend bool
    operator==(generator_sentinel s, const generator_iterator& it) noexcept {
        return (it == s);
    }

    friend bool
    operator!=(generator_sentinel s, const generator_iterator& it) noexcept {
        return it != s;
    }

    generator_iterator& operator++() {
        m_coroutine.resume();
        if (m_coroutine.done()) {
            m_coroutine.promise().rethrow_if_exception();
        }

        return *this;
    }

    // Need to provide post-increment operator to implement the 'Range' concept.
    void operator++(int) { (void)operator++(); }

    reference operator*() const noexcept {
        return m_coroutine.promise().value();
    }

    pointer operator->() const noexcept { return std::addressof(operator*()); }

   private:
    coroutine_handle m_coroutine;
};
} // namespace detail

template <typename T>
class [[nodiscard]] generator {
   public:
    using promise_type = promise::generator<T>;
    using iterator = detail::generator_iterator<T>;

    generator() = default;

    generator(generator && other) noexcept
      : m_coroutine(other.m_coroutine) {
        other.m_coroutine = nullptr;
    }

    generator(std::coroutine_handle<promise_type> coroutine) noexcept
      : m_coroutine(coroutine) {}

    generator(const generator& other) = delete;

    ~generator() {
        if (m_coroutine) {
            m_coroutine.destroy();
        }
    }

    generator& operator=(generator other) noexcept {
        swap(other);
        return *this;
    }

    iterator begin() {
        if (m_coroutine) {
            m_coroutine.resume();
            if (m_coroutine.done()) {
                m_coroutine.promise().rethrow_if_exception();
            }
        }

        return iterator {m_coroutine};
    }

    detail::generator_sentinel end() noexcept {
        return detail::generator_sentinel {};
    }

    void swap(generator & other) noexcept {
        std::swap(m_coroutine, other.m_coroutine);
    }

    std::coroutine_handle<promise_type> m_coroutine = nullptr;
};

template <typename T>
void swap(generator<T>& a, generator<T>& b) {
    a.swap(b);
}

template <typename FUNC, typename T>
generator<
    std::invoke_result_t<FUNC&, typename generator<T>::iterator::reference>>
fmap(FUNC func, generator<T> source) {
    for (auto&& value : source) {
        co_yield std::invoke(func, static_cast<decltype(value)>(value));
    }
}

template <class T>
auto begin(generator<T>& gen) {
    return gen.begin();
}
template <class T>
auto end(generator<T>& gen) {
    return detail::generator_iterator<T> {};
}
} // namespace conduit
