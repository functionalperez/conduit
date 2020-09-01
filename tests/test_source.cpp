#include "run_test.hpp"
#include <conduit/source.hpp>

future<std::string> test_source(std::string on_success) {
    auto coro = [&]() -> source<char> {
        for (char c : on_success) {
            co_yield c;
        }
    };

    std::string result;
    auto source = coro();
    while (auto c = co_await source) {
        result += *c;
    }
    co_return result;
}

RUN_CORO_TEST(test_source)