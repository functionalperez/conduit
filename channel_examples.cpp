#pragma once
#include <conduit/channel.hpp>
#include <conduit/generator.hpp>
#include <iostream>

template <bool is_suspended_initially = check_first>
generator<int, true, is_suspended_initially> counter(int initial, int max,
                                                     int inc) {
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
    for (countBy3.resume(); !countBy3.done(); countBy3.resume()) {
        std::cout << countBy3->value << ' ';
    }
    std::cout << '\n';
}
void example3() {
    auto countBy3 = counter<check_first>(0, 30, 3);
    int value = 0;
    while (countBy3 >> value) {
        std::cout << value << ' ';
    }
    std::cout << '\n';
}
void example4() {
    auto countBy3 = counter<resume_first>(0, 30, 3);
    int value = 0;
    while (countBy3 >> value) {
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
void example6() {
    auto countBy3 = counter(0, 30, 3);
    auto it = begin(countBy3);
    auto sentinal = end(countBy3);
    for (; it != sentinal; ++it) {
        std::cout << *it << ' ';
    }
    std::cout << '\n';
}

channel<int> sum_messages() {
    int sum = co_yield get_message;
    while (true) {
        sum += co_yield sum;
    }
}
void example_channel() {
    auto ch = sum_messages();
    for (int i = 0; i <= 10; i++) {
        int result = 0;
        ch << i >> result;
        std::cout << "Sum so far: " << result << '\n';
    }
}
void run() {

    example1();
    example2();
    example3();
    example4();
    example5();
    example6();
    example_channel();
}
