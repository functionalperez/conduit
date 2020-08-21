#pragma once
#include <conduit/stdlib_coroutine.hpp>

namespace conduit {
struct coro_sentinal {};

template <class Promise>
class coro_iterator {
    using handle = std::coroutine_handle<Promise>;
    handle coro;
    Promise& promise;

   public:
    coro_iterator(Promise& p) : coro(handle::from_promise(p)), promise(p) {}

    // Resumes the coroutine; returns reference to this iterator
    void operator++() { coro.resume(); }
    // Resumes the coroutine
    void operator++(int) { coro.resume(); }

    // Calls deref on the coroutine handle and returns the result
    // By default, this returns coro.promise().current_value;
    decltype(auto) operator*() noexcept { return promise.value(); }

    // Returns true iff the coroutine is done
    bool operator==(coro_sentinal) const noexcept { return coro.done(); }
    // Returns true iff the coroutine is not done
    bool operator!=(coro_sentinal) const noexcept { return !coro.done(); }
};
template <class promise>
coro_iterator(promise&) -> coro_iterator<promise>;

} // namespace conduit
