#include "count_allocs.hpp"

#include <conduit/coroutine.hpp>
#include <conduit/future.hpp>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

using namespace conduit;
std::thread alt_thread;

#define RUN_CORO_TEST(test)                                                    \
    int main() {                                                               \
        auto coro = []() -> conduit::coroutine {                               \
            std::string input = "" #test " succeeded";                         \
            auto coro = test(input);                                           \
            auto result = co_await coro;                                       \
            printf("Running test %s...\t", #test);                             \
            printf("%s\n", result.data());                                     \
            if (result != input) {                                             \
                std::exit(1);                                                  \
            }                                                                  \
        };                                                                     \
        try {                                                                  \
            coro();                                                            \
        } catch (std::exception & ex) {                                        \
            printf("\nException reached main. what(): %s\n", ex.what());       \
            if (alt_thread.joinable()) {                                       \
                alt_thread.join();                                             \
            }                                                                  \
            return 1;                                                          \
        }                                                                      \
        if (alt_thread.joinable())                                             \
            alt_thread.join();                                                 \
        printf("Number of allocs:   %i\n", number_of_allocs.load());           \
        printf("Number of deallocs: %i\n", number_of_deallocs.load());         \
        int num_to_show = alloc_record_size;                                   \
        if (num_to_show > number_of_allocs)                                    \
            num_to_show = number_of_allocs;                                    \
        printf("First %i allocations: \n", num_to_show);                       \
        for (int i = 0; i < num_to_show; i++) {                                \
            printf("[alloc %lu bytes]\n", alloc_record[i]);                    \
        }                                                                      \
        if (number_of_allocs != number_of_deallocs) {                          \
            printf("[memory leak]\n");                                         \
            return 1;                                                          \
        }                                                                      \
        return 0;                                                              \
    }

#define FAIL_TEST                                                              \
    int main() { return 1; }
