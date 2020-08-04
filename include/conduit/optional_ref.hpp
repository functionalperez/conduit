#pragma once
#include <concepts>
#include <cstddef>
#include <optional>
#include <utility>

namespace conduit {
template <class T>
struct optional_ref {
    T* pointer = nullptr;
    constexpr T* operator->() const noexcept { return pointer; }
    constexpr T& operator*() const& noexcept { return *pointer; }
    constexpr T& value() const& noexcept { return *pointer; }
    constexpr operator bool() const noexcept { return pointer != nullptr; }
    constexpr bool has_value() const noexcept { return pointer != nullptr; }
    template <std::convertible_to<T> U>
    constexpr T value_or(U&& default_value) const& {
        return pointer ? T(*pointer) : T(std::forward<U>(default_value));
    }
    template <std::convertible_to<T> U>
    constexpr T value_or(U&& default_value) && {
        return pointer ? T(std::move(*pointer))
                       : T(std::forward<U>(default_value));
    }
};

template <class T>
optional_ref(T&) -> optional_ref<T>;
template <class T>
optional_ref(T*) -> optional_ref<T>;
} // namespace conduit
