#include "run_test.hpp"
#include <conduit/async/on_suspend.hpp>

future<std::string> test_on_suspend(std::string on_success) {
    std::string result;
    co_await async::on_suspend{[&](std::coroutine_handle<> h) {
        result = on_success;
        h.resume();
    }};
    co_return result;
}

RUN_CORO_TEST(test_on_suspend)