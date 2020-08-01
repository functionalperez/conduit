#pragma once
#include <conduit/promise/helper.hpp>
#include <cstdio>

namespace conduit {
template <class Alloc>
concept allocator = requires(Alloc a, size_t size, void* pointer) {
    { a.alloc(size) }
    ->std::same_as<void*>;
    { Alloc::dealloc(pointer, size) }
    ->std::same_as<void>;
};
template <class Promise>
struct continuation : std::coroutine_handle<Promise> {
    using promise_type = Promise;
};
} // namespace conduit

namespace conduit::promise {
template <conduit::allocator Alloc>
struct continuation : initial_suspend_base<true>,
                      final_suspend_base<false>,
                      return_void_base<true> {
    using self = continuation;
    using result_type = conduit::continuation<self>;
    using handle_type = std::coroutine_handle<self>;
    result_type get_return_object() noexcept {
        puts("Getting return object");
        return {handle_type::from_promise(*this)};
    }
    void unhandled_exception() {}
    template <class... T>
    static void* operator new(size_t size, Alloc& alloc, T&&...) {
        return alloc.alloc(size);
    }
    template <class... T>
    static void* operator new(size_t size, Alloc* alloc, T&&...) {
        return alloc->alloc(size);
    }
    static void operator delete(void* pointer, size_t size) {
        puts("Calling delete");
        std::decay_t<Alloc>::dealloc(pointer, size);
    }
};
} // namespace conduit::promise

namespace conduit {
template<class Alloc>
continuation<promise::continuation<Alloc>> noop_continuation(Alloc* alloc) {
    co_return;
}
template<class Alloc>
continuation<promise::continuation<Alloc>> noop_continuation(Alloc& alloc) {
    co_return;
}
}