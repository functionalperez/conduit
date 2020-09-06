#include <conduit/coroutine.hpp>
#include <conduit/future.hpp>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>
#include <thread>

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
        return 0;                                                              \
    }

#define FAIL_TEST                                                              \
    int main() { return 1; }