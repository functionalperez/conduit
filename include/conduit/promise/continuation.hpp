#pragma once
#include <conduit/concepts.hpp>
#include <conduit/mixin/promise_parts.hpp>

namespace conduit::promise {
template <conduit::allocator Alloc>
struct continuation : mixin::GetReturnObject<continuation<Alloc>, false>,
                      mixin::UnhandledException<true>,
                      mixin::InitialSuspend<mixin::always>,
                      mixin::FinalSuspend<mixin::never>,
                      mixin::ReturnVoid {
    template <class... T>
    static void* operator new(size_t size, Alloc& alloc, T&&...) {
        return alloc.alloc(size);
    }
    template <class... T>
    static void* operator new(size_t size, Alloc* alloc, T&&...) {
        return alloc->alloc(size);
    }
    static void operator delete(void* pointer, size_t size) {
        std::decay_t<Alloc>::dealloc(pointer, size);
    }
};
} // namespace conduit::promise
