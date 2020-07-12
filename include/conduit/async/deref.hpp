#pragma once
#include <conduit/common.hpp>

namespace async {
template <class T>
struct deref {
    T& value;
    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<>) {}
    T await_resume() { return value; }
};
template <class T>
deref(T&) -> deref<T>;
} // namespace async
