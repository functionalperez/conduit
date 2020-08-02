#include <array>
#include <conduit/async/get_this_handle.hpp>
#include <conduit/async/on_suspend.hpp>
#include <conduit/co_void.hpp>
#include <conduit/continuation.hpp>
#include <conduit/future.hpp>
#include <conduit/source.hpp>
#include <conduit/task.hpp>
#include <conduit/void_coro.hpp>
#include <cstdlib>
#include <cstdio>
#include <vector>
using namespace conduit;

source<int> nums(int min, int max, int inc) {
    for(; min < max; min += inc) {
        co_yield min;
    }
}
void_coro do_stuff() {
    auto source = nums(0, 1000000000, 1);
    while(auto val = co_await source) {
        
    }
}

int main() {
    do_stuff();
}
