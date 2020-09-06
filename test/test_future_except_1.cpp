#include "run_test.hpp"
#include <stdexcept>

future<int> test_future_ex(std::string what) {
    throw std::logic_error(what);

    co_return 42;
}

future<std::string> test_future_except_1(std::string on_success) {
    try {
        auto coro = test_future_ex(on_success);
        auto val = co_await coro;
        co_return "No exception caught";
    } catch (std::logic_error& ex) {
        co_return ex.what();
    }
}

RUN_CORO_TEST(test_future_except_1);
