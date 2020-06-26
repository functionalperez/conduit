#include <iostream>
#include <minimal_promise.hpp>
#include <string_view>
#include "channel_examples.cpp"

void print_ordered(auto... args) {
    static int event = 1;
    std::cout << event << ")\t";
    (std::cout << ... << args);
    std::cout << '\n';
    event += 1;
}

generator<int> bar() {
    co_yield 3;
    co_yield {};
    co_yield 5;
}

minimal_coro foo() {
    print_ordered("Started coroutine");
    auto* promise = co_yield get_promise;
    print_ordered("Promise at ", (void*)promise);
    co_await std::suspend_always();
    print_ordered("Resumed coroutine");
}
int main() {
    run();
    auto coro = foo();
    print_ordered("Created coroutine with promise at ", (void*)(&coro.promise()));
    coro.resume();
    print_ordered("Coroutine suspends after reaching co_await");
    coro.resume();
    print_ordered("Coroutine complete");
    for(int i : bar()) {
        std::cout << i << '\n';
    }
}
/* Outputs:
1)	Created coroutine
2)	Started coroutine
3)	Coroutine suspends after reaching co_await
4)	Resumed coroutine
5)	Coroutine complete
*/
