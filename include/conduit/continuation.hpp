#pragma once
#include <conduit/promise/continuation.hpp>

namespace conduit {
template <mem::allocator Alloc>
struct continuation : std::coroutine_handle<promise::continuation<Alloc>> {
    using promise_type = promise::continuation<Alloc>;
    using super = std::coroutine_handle<promise_type>;

/*
Clang requires that a return object has a constructor which allows it to be
constructed from a handle, but having this constructor causes an internal
compiler error in gcc
*/
#ifdef __clang__
    continuation() = default;
    continuation(super handle) : super(handle) {}
    continuation(continuation const&) = default;
    continuation(continuation&&) = default;
#endif
};

template <mem::allocator Alloc>
continuation<Alloc> noop_continuation(Alloc* alloc) {
    co_return;
}

template <class Alloc>
continuation<Alloc> noop_continuation(Alloc& alloc) {
    co_return;
}
} // namespace conduit