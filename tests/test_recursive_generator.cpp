#include "run_test.hpp"
#include <conduit/recursive_generator.hpp>

auto example_recursive_generator(std::string_view s)
    -> recursive_generator<char> {
    if (s.empty()) {
        co_return tags::nothing;
    } else {
        co_yield s[0];
        co_return example_recursive_generator(s.substr(1));
    }
};
future<std::string> test_recursive_generator(std::string on_success) {
    auto g = example_recursive_generator(on_success);
    co_return std::string(begin(g), end(g));
}

RUN_CORO_TEST(test_recursive_generator)