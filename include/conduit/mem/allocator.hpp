#pragma once
#include <cstddef>

namespace conduit::mem {
// clang-format off
template <class Alloc>
concept allocator = requires(Alloc a, size_t size, void* pointer) {
    { a.alloc(size) } -> std::same_as<void*>;
    { Alloc::dealloc(pointer, size) };
};
// clang-format on

struct default_allocator {
    void* alloc(size_t size) { return ::new char[size]; }
    static void dealloc(void* pointer, size_t size) { delete[](char*) pointer; }
};
static_assert(allocator<default_allocator>,
              "mem::default_allocator must satisfy mem::allocator");

} // namespace conduit::mem