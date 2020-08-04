#pragma once
#include <conduit/common.hpp>

namespace conduit {
struct coro_sentinal {};

template <class Promise, bool is_noexcept = true>
class coro_iterator {
    std::coroutine_handle<Promise> coro;
    Promise& promise;

   public:
    coro_iterator(std::coroutine_handle<Promise> coro)
      : coro(coro), promise(coro.promise()) {}

    // Resumes the coroutine; returns reference to this iterator
    auto operator++() noexcept(is_noexcept) -> coro_iterator& {
        coro.resume();
        return *this;
    }
    // Resumes the coroutine
    void operator++(int) noexcept(is_noexcept) { coro.resume(); }

    // Calls deref on the coroutine handle and returns the result
    // By default, this returns coro.promise().current_value;
    decltype(auto) operator*() noexcept { return promise.value(); }

    // Returns true iff the coroutine is done
    bool operator==(coro_sentinal) noexcept(is_noexcept) { return coro.done(); }
    // Returns true iff the coroutine is not done
    bool operator!=(coro_sentinal) noexcept(is_noexcept) {
        return !coro.done();
    }
};
template <class promise>
coro_iterator(std::coroutine_handle<promise>) -> coro_iterator<promise>;

} // namespace conduit
