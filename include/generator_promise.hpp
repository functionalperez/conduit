#pragma once
#include <common.hpp>

template <class T, template<class> class return_object_t, bool is_coro_noexcept = true>
struct generator_promise {
   private:
    static T& mutable_T();
    static T const& const_T();
   public:
    constexpr static bool is_noexcept = is_coro_noexcept;
    constexpr static bool copy_T_noexcept =
        is_noexcept || noexcept(T(const_T()));
    constexpr static bool assign_T_noexcept =
        is_noexcept || noexcept(mutable_T() = const_T());

    using handle = std::coroutine_handle<generator_promise>;
    using return_object = return_object_t<generator_promise>;
    T current_value;
    static auto get_return_object_on_allocation_failure() noexcept {
        return return_object{nullptr};
    }
    auto get_return_object() noexcept {
        return return_object{handle::from_promise(*this)};
    }
    constexpr auto initial_suspend() noexcept { return std::suspend_always(); }
    constexpr auto final_suspend() noexcept { return std::suspend_always(); }
    [[noreturn]] void unhandled_exception() noexcept { std::terminate(); }
    constexpr void return_void() noexcept {}
    constexpr auto yield_value(T value) noexcept(assign_T_noexcept) {
        current_value = value;
        return std::suspend_always{};
    }
};