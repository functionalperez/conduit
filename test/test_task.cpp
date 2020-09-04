#include <conduit/task.hpp>
#include "run_test.hpp"

future<std::string> test_task(std::string on_success) {
    auto coro = [&](std::string& s) -> task {
        s = on_success;
        co_return;
    };

    std::string result;
    auto task = coro(result);
    // co_await coro(result);
    co_await task;
    co_return result;
}

RUN_CORO_TEST(test_task)