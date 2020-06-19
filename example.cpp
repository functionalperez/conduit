#include "include/generator_promise.hpp"
#include "include/unique_handle.hpp"
#include <iostream>

template <class T>
using generator_handle = unique_handle<generator_promise<T, unique_handle>>;

generator_handle<int> counter(int initial, int max, int inc) {
    for (; initial < max; initial += inc) {
        co_yield initial;
    }
}

void example1() {
    auto countBy3 = counter(0, 30, 3);
    for (; !countBy3.done(); countBy3.resume()) {
        std::cout << countBy3->value << ' ';
    }
    std::cout << '\n';
}
void example2() {
    auto countBy3 = counter(0, 30, 3);
    for (int i : countBy3) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
}
int main() {
    example1();
    example2();
}