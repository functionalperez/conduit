#pragma once
#include <common.hpp>
#include <utility>
#include <optional>

struct generator_sentinal {};

template <class T, class ReturnObject, bool is_noexcept = true>
struct promise {
    static T& mutable_T();
    static T const& const_T();
    constexpr static bool copy_T_noexcept =
        is_noexcept || noexcept(T(const_T()));
    constexpr static bool assign_T_noexcept =
        is_noexcept || noexcept(mutable_T() = const_T());

    using handle = std::coroutine_handle<promise>;
    T current_value;
    static auto get_return_object_on_allocation_failure() noexcept {
        return ReturnObject{nullptr};
    }
    auto get_return_object() noexcept {
        return ReturnObject{handle::from_promise(*this)};
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

template <class T, bool is_noexcept = true>
struct generator : private std::coroutine_handle<
                       promise<T, generator<T, is_noexcept>, is_noexcept>> {
    using promise_type = promise<T, generator<T, is_noexcept>, is_noexcept>;
    using handle = std::coroutine_handle<promise_type>;
    using handle::operator bool;
    using handle::done;
    using handle::resume;
    constexpr static bool copy_T_noexcept = promise_type::copy_T_noexcept;
    constexpr static bool assign_T_noexcept = promise_type::assign_T_noexcept;

    struct iterator {
        handle coro = nullptr;

        auto operator++() noexcept(is_noexcept) -> iterator& {
            coro.resume();
            return *this;
        }
        void operator++(int) noexcept(is_noexcept) {
            coro.resume();
        }
        T operator*() noexcept(copy_T_noexcept) {
            return coro.promise().current_value;
        }
        bool operator==(generator_sentinal) noexcept(is_noexcept) {
            return coro.done();
        }
        bool operator!=(generator_sentinal) noexcept(is_noexcept) {
            return !coro.done();
        }
    };

    bool move_next() noexcept(is_noexcept) {
        return handle::operator bool() ? (resume(), !done()) : false;
    }
    constexpr T current_value() noexcept(copy_T_noexcept) {
        return handle::promise().current_value;
    }
    constexpr generator() = default;
    constexpr generator(handle coro) : handle(coro) {}
    constexpr generator(generator const&) = delete;
    constexpr generator(generator&& rhs) noexcept : handle(rhs.get_handle()) {
        rhs = nullptr;
    }

    auto operator=(generator&& other) noexcept(is_noexcept) {
        generator temp;
        temp.swap(other); // Make other null
        swap(temp);       // Swap out current state with state from other
    }
    void destroy() {
        if (*this) {
            handle::destroy();
            handle::operator=(nullptr);  
        }
    }
    ~generator() noexcept(is_noexcept) {
        if (*this)
            handle::destroy();
    }
    iterator begin() noexcept(is_noexcept) { 
        resume();
        return {get_handle()}; 
    }
    std::optional<T> next() {
        resume();
        if(done()) {
            return {};
        }
        return {handle::promise().current_value};
    }
    bool operator>>(T& dest) {
        resume();
        if(done()) return false;
        dest = handle::promise().current_value;
        return true;
    }
    constexpr generator_sentinal end() noexcept { return {}; }
    void swap(generator& other) { std::swap(get_handle(), other.get_handle()); }

   private:
    constexpr handle& get_handle() noexcept { return *this; }
};