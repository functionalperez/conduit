#include <atomic>
#include <chrono>
#include <conduit/async/destroy.hpp>
#include <conduit/async/on_suspend.hpp>
#include <conduit/async/suspend_invoke.hpp>
#include <conduit/coroutine.hpp>
#include <conduit/future.hpp>
#include <conduit/generator.hpp>
#include <conduit/recursive_generator.hpp>
#include <conduit/source.hpp>
#include <conduit/task.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

using namespace conduit;
std::thread alt_thread;

int test_count = 0;
#define RUN_TEST(test)                                                         \
    {                                                                          \
        auto coro = test("" #test " succeeded");                               \
        std::cout << "Running test " << test_count++ << "...\t"                \
                  << co_await coro << std::endl;                               \
    }

future<std::string> test_coroutine(std::string on_success) {
    auto coro = [&](std::string& result) -> coroutine {
        result = on_success;
        co_return;
    };

    std::string result;
    coro(result);
    co_return result;
}

task inner_task(std::string& result, std::string const& on_success,
                std::string const& on_failure) {
    result = on_success;
    co_await async::destroy();
    result = on_failure;
}
task outer_task(std::string& result, std::string const& on_success,
                std::string const& on_failure) {
    task t = inner_task(result, on_success, on_failure);
    co_await t;
    result = on_failure;
}
future<std::string> test_destroy(std::string on_success) {
    auto coro = [](std::string& result, std::string const& on_success,
                   std::string const& on_failure) -> coroutine {
        auto t = outer_task(result, on_success, on_failure);
        co_await t;
        result = on_failure;
    };
    std::string result;
    coro(result, on_success, "async::destroy failed to destroy");

    co_return result;
}

future<std::string> test_exception_1(std::string on_success) {
    auto coro = [](const char* what) -> coroutine {
        throw std::logic_error(what);
        co_return;
    };

    try {
        coro(on_success.data());
        co_return "Failed test_exception_1: exception not thrown";
    } catch (std::exception& ex) {
        co_return ex.what();
    }
    co_return "Failed test_exception_1: exception not caught";
}

template <class F>
class invoke_on_destroy {
    [[no_unique_address]] F invoke;
    bool do_action;

   public:
    invoke_on_destroy() = delete;
    invoke_on_destroy(F const& action) : invoke(action), do_action(true) {}
    invoke_on_destroy(F&& action)
      : invoke(std::move(action)), do_action(true) {}
    invoke_on_destroy(invoke_on_destroy&& other) noexcept(
        std::is_nothrow_move_constructible_v<F>)
      : invoke(std::move(other.invoke)), do_action(other.do_action) {
        other.do_action = false;
    }
    void cancel() { do_action = false; }
    ~invoke_on_destroy() {
        if (do_action) {
            invoke();
        }
    }
};
template <class F>
invoke_on_destroy(F) -> invoke_on_destroy<F>;
future<std::string> test_exception_2(std::string on_success) {

    auto coro = [&](auto arg) -> coroutine {
        throw std::logic_error("Some error");
        co_return;
    };

    std::string result = "Result not set";
    auto action = [&]() { result = std::move(on_success); };
    try {
        coro(invoke_on_destroy(action));
        co_return "Failed test_exception_2: exception not caught";
    } catch (std::logic_error& err) {
        co_return result;
    }
    co_return "Failed test_exception_2: exception not caught";
}

future<std::string> test_future(std::string on_success) {
    co_return on_success;
}

future<std::string> test_generator(std::string on_success) {
    auto coro = [&]() -> generator<char> {
        for (char c : on_success) {
            co_yield c;
        }
    };

    auto g = coro();

    co_return std::string(begin(g), end(g));
}

future<std::string> test_on_suspend(std::string on_success) {
    std::string result;
    auto f = [](std::coroutine_handle<> h, std::string& result,
                std::string& on_success) {
        result = on_success;
        h.resume();
    };
    co_await async::on_suspend(f, result, on_success);
    co_return result;
}

auto example_recursive_generator(std::string_view s)
    -> recursive_generator<char> {
    if (s.empty()) {
        co_return tags::nothing;
    } else {
        co_yield s[0];
        co_return example_recursive_generator(s.substr(1));
    }
};
future<std::string> test_recursive_generator(std::string on_success) {
    auto g = example_recursive_generator(on_success);
    co_return std::string(begin(g), end(g));
}

future<std::string>
test_resume_on_alternate_thread(std::string on_success) {
    struct move_to_new_thread {
        constexpr bool await_ready() { return false; }
        void await_suspend(std::coroutine_handle<> h) {
            std::atomic_bool started = false;
            alt_thread = std::thread([&]() {
                started = true;
                h.resume();
            });
            while (!started) {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(10ms);
            }
        }
        void await_resume() noexcept {}
    };
    co_await move_to_new_thread{};
    co_return on_success;
}

future<std::string> test_source(std::string on_success) {
    auto coro = [&]() -> source<char> {
        for (char c : on_success) {
            co_yield c;
        }
    };

    std::string result;
    auto source = coro();
    while (auto c = co_await source) {
        result += *c;
    }
    co_return result;
}

future<std::string> test_suspend_invoke(std::string on_success) {
    std::string result;
    co_await async::suspend_invoke{[&](std::coroutine_handle<> h) {
        result = on_success;
        h.resume();
    }};
    co_return result;
}

future<std::string> test_task(std::string on_success) {
    auto coro = [&](std::string& s) -> task {
        s = on_success;
        co_return;
    };

    std::string result;
    auto task = coro(result);
    // co_await coro(result);
    co_await task;
    co_return result;
}

coroutine run_tests() {
    RUN_TEST(test_coroutine);
    RUN_TEST(test_destroy);
    RUN_TEST(test_exception_1);
    RUN_TEST(test_exception_2);
    RUN_TEST(test_future);
    RUN_TEST(test_generator);
    RUN_TEST(test_on_suspend);
    RUN_TEST(test_recursive_generator);
    RUN_TEST(test_resume_on_alternate_thread);
    RUN_TEST(test_source);
    RUN_TEST(test_suspend_invoke);
    RUN_TEST(test_task);
}

int main() {
    run_tests();
    if (alt_thread.joinable())
        alt_thread.join();
    std::cout << "finished\n";
}
