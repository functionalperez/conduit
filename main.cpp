#include <conduit/future.hpp>
#include <conduit/source.hpp>
#include <conduit/void_coro.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace conduit;

#define RUN_TEST(test) std::cout << "Running " #test "...\t" << test() << std::endl;

std::string test_void_coro() {
    std::string result = "Coroutine didn't run";
    [&]() -> void_coro {
        result = "Ok";
        co_return;
    }();
    return result;
}
std::string test_future_1() {
    std::string result = "Corutine didn't run";
    auto f = []() -> future<int> { co_return 10; };
    auto g = f();
    [&]() -> void_coro {
        int value = 1;
        value = co_await g;
        if(value == 10) {
            result = "Ok";
        } else {
            result = "Incorrect value obtained";
        }
    }();
    return result;
}
std::string test_source() {
    std::string result = "Failed";
    [&]() -> void_coro {
        auto g = []() -> source<int> {
            for(int i = 0; i < 10; i++) {
                co_yield i;
            }
        }();
        auto expected = std::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::vector<int> values;
        while(auto val = co_await g) {
            values.push_back(*val);
        }
        if(values == expected) {
            result = "Ok";
        }
    }();
    return result;
}

int main() {
    RUN_TEST(test_void_coro);
    RUN_TEST(test_future_1);
    RUN_TEST(test_source);

    std::cout << "finished\n";
}
