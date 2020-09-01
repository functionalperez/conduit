#include "run_test.hpp"
#include <conduit/async/on_suspend.hpp>

future<std::string> test_suspend_invoke(std::string on_success) {
    std::string result;
    co_await async::suspend_invoke{[&](std::coroutine_handle<> h) {
        result = on_success;
        h.resume();
    }};
    co_return result;
}

RUN_CORO_TEST(test_suspend_invoke)