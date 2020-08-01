#include <array>
#include <conduit/async/get_this_handle.hpp>
#include <conduit/async/on_suspend.hpp>
#include <conduit/co_void.hpp>
#include <conduit/continuation.hpp>
#include <conduit/future.hpp>
#include <conduit/source.hpp>
#include <conduit/task.hpp>
#include <conduit/void_coro.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>
using namespace conduit;

void_coro foo() {
    // It is safe to resume or destroy the calling coroutine when using async::on_suspend
    auto op = [](auto callback, auto nums) {
        printf("Callback on %p\n", callback.address());
        for(int i : nums) {
            std::cout << i << '\n';
        }
        callback.destroy();
    };
    co_await async::on_suspend(op, std::vector{1, 2, 3, 4, 5});
    puts("Done");
}

int main() {
    unique_handle<std::coroutine_handle<>> h{nullptr};

    foo();
}
