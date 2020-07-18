#include <conduit/co_void.hpp>
#include <conduit/source.hpp>
#include <conduit/task.hpp>
#include <conduit/void_coro.hpp>
#include <iostream>

using namespace conduit;

source<int> bar() {
    co_yield 10;
    co_yield 20;
    co_yield 30;
}
task simple_task() {
    std::cout << "Doing task\n";
    co_return;
}

void_coro foo() {
    auto sauce = bar();
    while (auto value = co_await async::on_coro{sauce}) {
        std::cout << *value << '\n';
    }
    std::cout << "No more values\n" << std::flush;
    co_await async::on_coro{simple_task()};
    std::cout << "Completed task\n";
}
void_coro tiny() {
    co_return ;
}
int main() {
    foo();
    std::cout << "Exiting main\n";
}
