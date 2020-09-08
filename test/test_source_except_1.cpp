#include "run_test.hpp"
#include <stdexcept>
#include <conduit/source.hpp>

source<int> test_source_ex(std::string what) {
    throw std::logic_error(what);

    co_yield 42;
}

future<std::string> test_source_except_1(std::string on_success) {
    try {
        auto coro = test_source_ex(on_success);
        auto val = co_await coro;
        co_return "No exception caught";
    } catch (std::logic_error& ex) {
        co_return ex.what();
    }
}

RUN_CORO_TEST(test_source_except_1);
