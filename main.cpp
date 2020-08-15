#include <conduit/source.hpp>
#include <conduit/void_coro.hpp>
#include <conduit/future.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
using namespace conduit;

source<int> nums(int min, int inc) {
    for (;; min += inc) {
        co_yield min;
    }
}
source<int> yield0_to_10() {
    for(int i = 0; i < 10; i++) {
        co_yield i;
    }
}
optional_future<int> get_future() {
    co_return 12345;
}
void_coro run(auto coro) {
    int count = 0;
    while (auto val = co_await coro) {
        auto value = *val;
        std::cout << value << '\n';
        ++count;
        if (count == 100 || coro.done()) break;
    }
    puts("Done");
}

int main() {
    run(nums(0, 1));
    run(yield0_to_10());
    run(get_future());

    std::cout << "finished\n";
}
