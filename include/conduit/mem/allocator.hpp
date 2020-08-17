#pragma once
#include <conduit/concepts.hpp>
#include <cstddef>
#include <cstdio>

namespace conduit::mem {
// clang-format off
template <class Alloc>
concept allocator = requires(Alloc a, size_t size, void* pointer) {
    { a.alloc(size) } -> same_as<void*>;
    { Alloc::dealloc(pointer, size) };
};
// clang-format on

struct default_allocator {
    void* alloc(size_t size) { return ::new char[size]; }
    static void dealloc(void* pointer, size_t size) { delete[](char*) pointer; }
};
template <size_t buffer_size>
struct static_allocator {
    using self = static_allocator;
    alignas(void*) mutable char buffer[buffer_size]{};

    void* alloc(size_t size) {
        if (size <= sizeof(buffer)) {
            return buffer;
        } else {
            return new char[size]{};
        }
    }
    static void dealloc(void* addr, size_t size) {
        if (size > sizeof(buffer)) {
            delete[](char*) addr;
        }
    }
};

template <class Callback, size_t buffer_size>
struct static_callback_allocator {
    using self = static_callback_allocator;
    alignas(void*) mutable char buffer[buffer_size]{};
    [[no_unique_address]] Callback callback;

    void* alloc(size_t size) {
        if (size <= sizeof(buffer)) {
            return buffer;
        } else {
            constexpr size_t offset = sizeof(self*);
            char* data = new char[offset + size];
            new (data) self*(this);
            return offset + data;
        }
    }
    static void dealloc(void* addr, size_t size) {
        if (size <= sizeof(buffer)) {
            puts("Deallocating static");
            self* pointer_to_allocator = (self*)addr;
            pointer_to_allocator->callback();
        } else {
            puts("Deallocating dynamic");
            constexpr size_t offset = sizeof(self*);
            self* pointer_to_allocator = *(self**)((char*)addr - offset);
            delete[]((char*)addr - offset);
            pointer_to_allocator->callback();
        }
    }
};

// On recommendation of shadow, we alias it to linda
template <class Callback, size_t buffer_size>
using linda = static_callback_allocator<Callback, buffer_size>;
// Linda is a monster
} // namespace conduit::mem