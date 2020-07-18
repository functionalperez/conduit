#include <conduit/co_void.hpp>
#include <conduit/void_coro.hpp>
#include <iostream>

using conduit::co_void;

co_void foo() {
    std::cout << "Hello, world!\n";
    co_await std::suspend_always{};
    std::cout << "Goodbye, world\n";
}
conduit::void_coro evil_coro(std::coroutine_handle<conduit::promise::void_coro>& handle) {
    handle = co_yield conduit::get_handle;

    std::cout << "Hello, world!\n";

    co_await std::suspend_always{};

    std::cout << "Goodbye, world!\n";
}

int main() {
    std::coroutine_handle<conduit::promise::void_coro> handle;
    bool success = evil_coro(handle);
    if(!success) {
        std::cout << "Failed\n";
        return 1;
    }

    std::cout << "[Paused]\n";

    handle.resume();

}
