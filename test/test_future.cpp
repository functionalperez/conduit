#include "run_test.hpp"

future<std::string> test_future(std::string on_success) {
    co_return on_success;
}

RUN_CORO_TEST(test_future)