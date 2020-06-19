
struct coro_sentinal {};

constexpr auto deref_current_value = [](auto coroutine_handle) -> decltype(auto) {
    return coroutine_handle.promise().current_value;
};
using deref_current_value_t = decltype(deref_current_value);
template <class handle, class deref_fn = deref_current_value_t, bool is_noexcept = true>
struct coro_iterator {
    handle coro = nullptr;
    [[no_unique_address]] deref_fn deref = {};

    auto operator++() noexcept(is_noexcept) -> coro_iterator& {
        coro.resume();
        return *this;
    }
    void operator++(int) noexcept(is_noexcept) { coro.resume(); }
    decltype(auto) operator*() noexcept(noexcept(deref(coro))) {
        return deref(coro);
    }
    bool operator==(coro_sentinal) noexcept(is_noexcept) {
        return coro.done();
    }
    bool operator!=(coro_sentinal) noexcept(is_noexcept) {
        return !coro.done();
    }
};
template<class handle>
coro_iterator(handle) -> coro_iterator<handle>;
template<class handle, class deref>
coro_iterator(handle, deref) -> coro_iterator<handle, deref>;