#include <conduit/async/on_suspend.hpp>
#include <conduit/coroutine.hpp>
#include <conduit/future.hpp>
#include <conduit/generator.hpp>
#include <conduit/recursive_generator.hpp>
#include <conduit/source.hpp>
#include <conduit/task.hpp>
#include <conduit/async/destroy.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using namespace conduit;

int test_count = 0;
#define RUN_TEST(test)                                                         \
    {                                                                          \
        auto coro = test("" #test " succeeded");                               \
        std::cout << "Running test " << test_count++ << "...\t"                \
                  << co_await coro << std::endl;                               \
    }

future<std::string> test_coroutine(std::string on_success) {
    auto coro = [&](std::string& result) -> coroutine {
        result = on_success;
        co_return;
    };

    std::string result;
    coro(result);
    co_return result;
}

task inner_task(std::string& result, std::string const& on_success, std::string const& on_failure) {
    result = on_success;
    co_await async::destroy();
    result = on_failure;
}
task outer_task(std::string& result, std::string const& on_success, std::string const& on_failure) {
    task t = inner_task(result, on_success, on_failure);
    co_await t;
    result = on_failure;
}
future<std::string> test_destroy(std::string on_success) {
    auto coro = [](std::string& result, std::string const& on_success, std::string const& on_failure) -> coroutine {
        auto t = outer_task(result, on_success, on_failure);
        co_await t;
        result = on_failure;
    };
    std::string result;
    coro(result, on_success, "async::destroy failed to destroy");

    co_return result;
}

future<std::string> test_future(std::string on_success) {
    co_return on_success;
}

future<std::string> test_generator(std::string on_success) {
    auto coro = [&]() -> generator<char> {
        for (char c : on_success) {
            co_yield c;
        }
    };

    auto g = coro();

    co_return std::string(begin(g), end(g));
}


future<std::string> test_on_suspend(std::string on_success) {
    std::string result;
    auto f = [](std::coroutine_handle<> h, std::string& result, std::string& on_success) {
        result = on_success;
        h.resume();
    };
    co_await async::make_on_suspend(f, result, on_success);
    co_return result;
}

auto example_recursive_generator(std::string_view s)
    -> recursive_generator<char> {
    if (s.empty()) {
        co_return nothing;
    } else {
        co_yield s[0];
        co_return example_recursive_generator(s.substr(1));
    }
};
future<std::string> test_recursive_generator(std::string on_success) {
    auto g = example_recursive_generator(on_success);
    co_return std::string(begin(g), end(g));
}

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

future<std::string> test_task(std::string on_success) {
    auto coro = [&](std::string& s) -> task {
        s = on_success;
        co_return;
    };

    std::string result;
    auto task = coro(result);
    // co_await coro(result);
    co_await task;
    co_return result;
}

coroutine run_tests() {
    RUN_TEST(test_coroutine);
    RUN_TEST(test_destroy);
    RUN_TEST(test_future);
    RUN_TEST(test_generator);
    RUN_TEST(test_on_suspend);
    RUN_TEST(test_recursive_generator);
    RUN_TEST(test_source);
    RUN_TEST(test_task);
}

int main() {
    run_tests();
    std::cout << "finished\n";
}
