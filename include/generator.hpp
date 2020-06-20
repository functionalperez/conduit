#pragma once
#include <generator_promise.hpp>
#include <unique_handle.hpp>

template <class T, bool is_noexcept = true, bool generator_mode = check_first>
using generator = unique_handle<
    generator_promise<T, unique_handle, is_noexcept, generator_mode>>;

template <class T, bool is_noexcept>
bool operator>>(generator<T, is_noexcept, check_first>& g,
                T& value) noexcept(is_noexcept) {
    if (g.done())
        return false;
    value = g->value;
    g.resume();
    return true;
}
template <class T, bool is_noexcept>
bool operator>>(generator<T, is_noexcept, resume_first>& g,
                T& value) noexcept(is_noexcept) {
    g.resume();
    if (g.done())
        return false;
    value = g->value;
    return true;
}
