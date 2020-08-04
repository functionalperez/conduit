#include <array>
#include <conduit/async/get_this_handle.hpp>
#include <conduit/async/on_suspend.hpp>
#include <conduit/co_void.hpp>
#include <conduit/continuation.hpp>
#include <conduit/future.hpp>
#include <conduit/source.hpp>
#include <conduit/task.hpp>
#include <conduit/void_coro.hpp>
#include <conduit/generator.hpp>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace conduit;

template <class Coro>
Coro nums(int min, int inc) {
    for (;; min += inc) {
        co_yield min;
    }
}
void do_stuff() {
    auto source = nums<generator<int>>(0, 1);
    for(int i : source) {
        printf("%i\n", i);
        if(i > 100) break;
    }
    puts("Done");
}

int main() { do_stuff(); }
