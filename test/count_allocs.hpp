#pragma once
#include <atomic>
#include <cstdlib>
#include <new>

std::atomic_int number_of_allocs = 0;
std::atomic_int number_of_deallocs = 0;
constexpr int alloc_record_size = 256;
size_t alloc_record[alloc_record_size];

void* operator new(std::size_t size) {
    int alloc_id = number_of_allocs++;
    if (alloc_id < alloc_record_size)
        alloc_record[alloc_id] = size;
    void* p = malloc(size);
    if (!p)
        throw std::bad_alloc();
    return p;
}

void* operator new[](std::size_t size) {
    int alloc_id = number_of_allocs++;
    if (alloc_id < alloc_record_size)
        alloc_record[alloc_id] = size;
    void* p = malloc(size);
    if (!p)
        throw std::bad_alloc();
    return p;
}

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept {
    int alloc_id = number_of_allocs++;
    if (alloc_id < alloc_record_size)
        alloc_record[alloc_id] = size;
    return malloc(size);
}
void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
    int alloc_id = number_of_allocs++;
    if (alloc_id < alloc_record_size)
        alloc_record[alloc_id] = size;
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    free(ptr);
    number_of_deallocs++;
}
void operator delete(void* ptr, const std::nothrow_t&) noexcept {
    free(ptr);
    number_of_deallocs++;
}
void operator delete[](void* ptr) noexcept {
    free(ptr);
    number_of_deallocs++;
}
void operator delete[](void* ptr, const std::nothrow_t&) noexcept {
    free(ptr);
    number_of_deallocs++;
}
