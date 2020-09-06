#include "run_test.hpp"
#include <conduit/generator.hpp>

future<std::string> test_generator(std::string on_success) {
    auto coro = [&]() -> generator<char> {
        for (char c : on_success) {
            co_yield c;
        }
    };

    auto g = coro();

    co_return std::string(begin(g), end(g));
}

RUN_CORO_TEST(test_generator)