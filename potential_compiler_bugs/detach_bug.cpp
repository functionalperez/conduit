#include <coroutine>
#include <exception>

// compile with:
// g++-10 -std=c++20 -fcoroutines detach_bug.cpp

struct promise {
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() { return {}; }
    void return_void() {}
    bool get_return_object() { return true; }
    static bool get_return_object_on_allocation_failure() { return false; }
    [[noreturn]] void unhandled_exception() { std::terminate(); }
};

struct result {
    bool success;
    result(bool success) : success(success) {}
    using promise_type = promise;
};

result foo() {
    co_return; 
}

int main() {
    foo();
}
