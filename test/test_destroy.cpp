#include "run_test.hpp"
#include <conduit/task.hpp>
#include <conduit/async/destroy.hpp>

task inner_task(std::string& result, std::string const& on_success,
                std::string const& on_failure) {
    result = on_success;
    co_await async::destroy();
    result = on_failure;
}
task outer_task(std::string& result, std::string const& on_success,
                std::string const& on_failure) {
    task t = inner_task(result, on_success, on_failure);
    co_await t;
    result = on_failure;
}
future<std::string> test_destroy(std::string on_success) {
    auto coro = [](std::string& result, std::string const& on_success,
                   std::string const& on_failure) -> coroutine {
        auto t = outer_task(result, on_success, on_failure);
        co_await t;
        result = on_failure;
    };
    std::string result;
    coro(result, on_success, "async::destroy failed to destroy");

    co_return result;
}

RUN_CORO_TEST(test_destroy);