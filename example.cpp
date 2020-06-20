#include "include/generator_promise.hpp"
#include "include/unique_handle.hpp"
#include <iostream>
#include <generator.hpp>

template<bool is_suspended_initially = check_first>
generator<int, true, is_suspended_initially> counter(int initial, int max, int inc) {
    for (; initial < max; initial += inc) {
        co_yield initial;
    }
}

void example1() {
    auto countBy3 = counter<check_first>(0, 30, 3);
    for (; !countBy3.done(); countBy3.resume()) {
        std::cout << countBy3->value << ' ';
    }
    std::cout << '\n';
}
void example2() {
    auto countBy3 = counter<resume_first>(0, 30, 3);
    for(countBy3.resume(); !countBy3.done(); countBy3.resume()) {
        std::cout << countBy3->value << ' ';
    }
    std::cout << '\n';
}
void example3() {
    auto countBy3 = counter<check_first>(0, 30, 3);
    int value = 0;
    while(countBy3 >> value) {
        std::cout << value << ' ';
    }
    std::cout << '\n';
}
void example4() {
    auto countBy3 = counter<resume_first>(0, 30, 3);
    int value = 0;
    while(countBy3 >> value) {
        std::cout << value << ' ';
    }
    std::cout << '\n';
}
void example5() {
    auto countBy3 = counter(0, 30, 3);
    for (int i : countBy3) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
}
int main() {
    example1();
    example2();
    example3();
    example4();
    example5();
}