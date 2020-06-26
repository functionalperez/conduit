#pragma once
#include <common.hpp>

template <class T>
struct awaitable_reference {
    T& value;
    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<>) {}
    T await_resume() { return value; }
};
template <class T>
awaitable_reference(T&) -> awaitable_reference<T>;
