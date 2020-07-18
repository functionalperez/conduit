#pragma once
#include <concepts>
#include <cstddef>
#include <utility>
#include <optional>

namespace conduit {
template <class T>
class optional_ref {
    T* pointer = nullptr;
   public:
    optional_ref() = default;
    optional_ref(optional_ref const&) = default;
    optional_ref(optional_ref&&) = default;
    constexpr optional_ref(T& value) : pointer(&value) {}
    constexpr optional_ref(std::optional<T>& value) : pointer(value ? value.operator->() : nullptr) {}
    constexpr optional_ref(std::nullptr_t) : pointer(nullptr) {}
    constexpr T* operator->() const noexcept { return pointer; }
    constexpr T& operator*() const& noexcept { return *pointer; }
    constexpr T&& operator*() && noexcept { return std::move(*pointer); }
    constexpr T& value() const& noexcept { return *pointer; }
    constexpr T&& value() && noexcept { return std::move(*pointer); }
    constexpr operator bool() const noexcept { return pointer != nullptr; }
    constexpr bool has_value() const noexcept { return pointer != nullptr; }
    template <std::convertible_to<T> U>
    constexpr T value_or(U&& default_value) const& {
        return pointer ? T(*pointer) : T(std::forward<U>(default_value));
    }
    template <std::convertible_to<T> U>
    constexpr T value_or(U&& default_value) && {
        return pointer ? T(std::move(*pointer)) : T(std::forward<U>(default_value));
    }
};
template<class T>
optional_ref(T&) -> optional_ref<T>;
template<class T>
optional_ref(std::optional<T>&) -> optional_ref<T>;
} // namespace conduit
