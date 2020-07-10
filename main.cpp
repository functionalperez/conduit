#include <minimal_promise.hpp>
#include <recursive_generator.hpp>

#include <task.hpp>

#include "channel_examples.cpp"

#include <iostream>
#include <string_view>

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

recursive_generator<long> recursive_fib(int num, long f1 = 1, long f2 = 1) {
    if (num == 0) {
        co_return nothing;
    }
    co_yield f1;
    co_return recursive_fib(num - 1, f2, f1 + f2);
}
auto fib(int num, long f1 = 1, long f2 = 1) -> recursive_generator<long> {
    co_yield f1;
    co_return num == 1 ? nothing : (fib(num - 1, f2, f1 + f2));
}
auto f(long f1 = 1, long f2 = 1) -> recursive_generator<long> {
    co_yield f1;
    co_return f(f2, f1 + f2);
}
auto nums(int initial = 0) -> recursive_generator<long> {
    co_yield initial;
    co_return nums(initial + 1);
}

future<int> t1() {
    std::cout << "Doing t1\n";
    co_return 4;
}
future<int> t2() {
    std::cout << "Recieved " << co_await t1() << '\n';
    co_return 5;
}
int main() {
    auto t2_ = t2();
    t2_.resume();

    auto r = fib(10);
    while (!r.done()) {
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
    for (int i : bar()) {
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
