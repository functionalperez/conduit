#include "run_test.hpp"
#include <stdexcept>

future<std::string> test_coroutine_except_1(std::string on_success) {
    auto coro = [](const char* what) -> coroutine {
        throw std::logic_error(what);
        co_return;
    };

    try {
        coro(on_success.data());
        co_return "Failed test_coroutine_except_1: exception not thrown";
    } catch (std::exception& ex) {
        co_return ex.what();
    }
    co_return "Failed test_coroutine_except_1: exception not caught";
}

RUN_CORO_TEST(test_coroutine_except_1)
