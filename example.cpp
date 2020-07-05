#include <iostream>
#include <minimal_promise.hpp>
#include <string_view>
#include "channel_examples.cpp"
#include <recursive_generator.hpp>

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

recursive_generator<int> recursive_fib(int f1 = 1, int f2 = 1) {
    co_yield f1;
    co_return recursive_fib(f2, f1 + f2);
}
recursive_generator<int> r2() {
    co_yield 1;
    co_yield 2; 
    co_return;
}
int main() {
    auto r = recursive_fib();
    std::cout << "Should have return object at " << (void*)&r << '\n';
    for(int i = 0; i < 100; i++) {
        std::cout << r->value << '\n';
        r.resume();
    }
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
