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
#include <conduit/util/unique_handle.hpp>
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
  : public mixin::InitialSuspend<false>
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
} // namespace promise

template <typename T>
class [[nodiscard]] generator : unique_handle<promise::generator<T>> {
    using super = unique_handle<promise::generator<T>>;

   public:
    using promise_type = promise::generator<T>;
    using iterator = coro_iterator<std::coroutine_handle<promise_type>>;

    using super::super;
    generator() = default;
    generator(generator &&) = default;
    generator(const generator& other) = delete;

    generator& operator=(generator other) noexcept {
        super::swap(other);
        return *this;
    }

    iterator begin() const {
        auto coro = super::get();
        if (coro && coro.done()) {
            coro.promise().rethrow_if_exception();
        }

        return iterator {coro};
    }

    iterator end() const noexcept { return {}; }
};

template <class T>
auto begin(generator<T>& gen) {
    return gen.begin();
}
template <class T>
auto end(generator<T>& gen) {
    return gen.end();
}
} // namespace conduit
