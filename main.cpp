#include <conduit/future.hpp>
#include <conduit/source.hpp>
#include <conduit/task.hpp>
#include <conduit/coroutine.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace conduit;

#define RUN_TEST(test)                                                         \
    std::cout << "Running " #test "...\t" << co_await test(#test " succeeded") << std::endl;

future<std::string> test_coroutine(std::string on_success) {
    auto coro = [=](std::string& result) -> coroutine {
        result = on_success;
        co_return;
    };

    std::string result;
    coro(result);
    co_return result;
}
future<std::string> test_future(std::string on_success) {
    co_return on_success;
}
future<std::string> test_source(std::string on_success) {
    auto coro = [=]() -> source<char> {
        for (char c : on_success) {
            co_yield c;
        }
    };

    std::string result;
    auto source = coro();
    while(auto c = co_await source) {
        result += *c;
    }
    co_return result;
}

future<std::string> test_task(std::string on_success) {
    auto coro = [=](std::string& s) -> task {
        s = on_success;
        co_return;
    };

    std::string result;
    co_await coro(result);
    co_return result;
}
coroutine run_tests() {
    RUN_TEST(test_coroutine);
    RUN_TEST(test_future);
    RUN_TEST(test_source);
    RUN_TEST(test_task);
}

int main() {
    run_tests();
    std::cout << "finished\n";
}
