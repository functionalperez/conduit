#include "run_test.hpp"

future<std::string> test_coroutine(std::string on_success) {
    auto coro = [&](std::string& result) -> coroutine {
        result = on_success;
        co_return;
    };

    std::string result;
    coro(result);
    co_return result;
}

RUN_CORO_TEST(test_coroutine)