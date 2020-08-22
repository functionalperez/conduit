#pragma once
#include <conduit/iterator.hpp>
#include <conduit/promise/generator.hpp>
#include <conduit/unique_handle.hpp>

namespace conduit {
template <class T>
using generator = unique_handle<promise::generator<T>>;

template<class T>
auto begin(generator<T>& g) -> coro_iterator<std::coroutine_handle<promise::generator<T>>> {
    return {g.get()};
}
template<class T>
auto end(generator<T>& g) -> coro_iterator<std::coroutine_handle<promise::generator<T>>> {
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
