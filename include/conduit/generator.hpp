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

#include <conduit/mixin/promise_parts.hpp>
#include <conduit/util/iterator.hpp>
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
class generator
  : public mixin::InitialSuspend<true>
  , public mixin::FinalSuspend<true>
  , public mixin::ExceptionHandler {
   public:
    using value_type = std::remove_reference_t<T>;
    using reference_type =
        std::conditional_t<std::is_reference_v<T>, T, T const&>;
    using pointer_type = std::remove_reference_t<reference_type>*;
    using coroutine_handle = std::coroutine_handle<generator>;

   private:
    pointer_type value_ptr;

   public:
    generator() = default;

    auto get_return_object() noexcept {
        return coroutine_handle::from_promise(*this);
    }

    std::suspend_always yield_value(reference_type value) noexcept {
        value_ptr = std::addressof(value);
        return {};
    }

    void return_void() noexcept {}

    reference_type value() const noexcept { return *value_ptr; }

    // Don't allow any use of 'co_await' inside the generator coroutine.
    template <typename U>
    std::suspend_never await_transform(U&& value) = delete;
};
} // namespace promise

template <typename T>
class [[nodiscard]] generator {
   public:
    using promise_type = promise::generator<T>;
    using iterator = coro_iterator<std::coroutine_handle<promise_type>>;

    generator() = default;
    generator(generator && other) noexcept
      : coro(other.m_coroutine) {
        other.m_coroutine = nullptr;
    }

    generator(std::coroutine_handle<promise_type> coroutine) noexcept
      : coro(coroutine) {}

    generator(const generator& other) = delete;

    ~generator() {
        if (coro) {
            coro.destroy();
        }
    }

    generator& operator=(generator other) noexcept {
        swap(other);
        return *this;
    }

    iterator begin() {
        if (coro) {
            coro.resume();
            if (coro.done()) {
                coro.promise().rethrow_if_exception();
            }
        }

        return iterator {coro};
    }

    iterator end() noexcept { return {}; }

    void swap(generator & other) noexcept {
        std::swap(coro, other.m_coroutine);
    }

    std::coroutine_handle<promise_type> coro = nullptr;
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
    return gen.end();
}
} // namespace conduit
