#include <minimal_promise.hpp>
#include <iostream>
#include <string_view>

void print_ordered(std::string_view s) {
    static int event = 1;
    std::cout << event << ")\t" << s << '\n';
    event += 1;
}

minimal_coro foo() {
    print_ordered("Started coroutine");
    co_await std::suspend_always();
    print_ordered("Resumed coroutine");
}
int main() {
    auto coro = foo();
    print_ordered("Created coroutine");
    coro.resume();
    print_ordered("Coroutine suspends after reaching co_await");
    coro.resume();
    print_ordered("Coroutine complete");
}
/* Outputs:
1)	Created coroutine
2)	Started coroutine
3)	Coroutine suspends after reaching co_await
4)	Resumed coroutine
5)	Coroutine complete
*/