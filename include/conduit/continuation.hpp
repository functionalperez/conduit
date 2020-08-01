#pragma once
#include <conduit/promise/continuation.hpp>

namespace conduit {
template <class Promise>
struct continuation : std::coroutine_handle<Promise> {
    using promise_type = Promise;
    using super = std::coroutine_handle<Promise>;

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

template <conduit::allocator Alloc>
continuation<promise::continuation<Alloc>> noop_continuation(Alloc* alloc) {
    co_return;
}

template <class Alloc>
continuation<promise::continuation<Alloc>> noop_continuation(Alloc& alloc) {
    co_return;
}
} // namespace conduit