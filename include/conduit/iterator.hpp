#pragma once

namespace conduit {
struct coro_sentinal {};

constexpr auto deref_current_value = [](auto coroutine_handle) -> decltype(auto) {
    return *coroutine_handle.promise().value;
};
using deref_current_value_t = decltype(deref_current_value);
template <class handle, class deref_fn = deref_current_value_t, bool is_noexcept = true>
struct coro_iterator {
    handle coro = nullptr;
    // Used to implement dereference
    [[no_unique_address]] deref_fn deref = {};

    // Resumes the coroutine; returns reference to this iterator
    auto operator++() noexcept(is_noexcept) -> coro_iterator& {
        coro.resume();
        return *this;
    }
    // Resumes the coroutine
    void operator++(int) noexcept(is_noexcept) { coro.resume(); }

    // Calls deref on the coroutine handle and returns the result
    // By default, this returns coro.promise().current_value;
    decltype(auto) operator*() noexcept(noexcept(deref(coro))) { return deref(coro); }

    // Returns true iff the coroutine is done
    bool operator==(coro_sentinal) noexcept(is_noexcept) { return coro.done(); }
    // Returns true iff the coroutine is not done
    bool operator!=(coro_sentinal) noexcept(is_noexcept) { return !coro.done(); }
};
template <class handle>
coro_iterator(handle) -> coro_iterator<handle>;
template <class handle, class deref>
coro_iterator(handle, deref) -> coro_iterator<handle, deref>;

} // namespace conduit
