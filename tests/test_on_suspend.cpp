#include "run_test.hpp"
#include <conduit/async/on_suspend.hpp>

future<std::string> test_on_suspend(std::string on_success) {
    std::string result;
    auto f = [](std::coroutine_handle<> h, std::string& result,
                std::string& on_success) {
        result = on_success;
        h.resume();
    };
    co_await async::on_suspend(f, result, on_success);
    co_return result;
}

RUN_CORO_TEST(test_on_suspend);