#include "run_test.hpp"

future<std::string> test_resume_on_alternate_thread(std::string on_success) {
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

RUN_CORO_TEST(test_resume_on_alternate_thread)