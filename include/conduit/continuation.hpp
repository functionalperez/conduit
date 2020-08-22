#pragma once
#include <conduit/mem/allocator.hpp>
#include <conduit/mixin/promise_parts.hpp>

namespace conduit::promise {
template <mem::allocator Alloc>
struct continuation : mixin::GetReturnObject<continuation<Alloc>, false>,
                      mixin::UnhandledException<true>,
                      mixin::InitialSuspend<mixin::always>,
                      mixin::FinalSuspend<mixin::never>,
                      mixin::ReturnVoid,
                      mixin::NewAndDelete<Alloc> {};
} // namespace conduit::promise


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

// On x86-64, noop_continuation_size is 24 on Clang
// And 48 on GCC. I don't know what it is on other compilers
#ifdef __clang__
constexpr size_t noop_continuation_size = sizeof(void*) * 3;
#else
constexpr size_t noop_continuation_size = sizeof(void*) * 6;
#endif
} // namespace conduit