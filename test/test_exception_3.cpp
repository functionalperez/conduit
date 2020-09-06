#include "run_test.hpp"
#include <stdexcept>

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

template <class Guard>
future<int> test_future_ex(Guard guard) {
    throw std::logic_error("Shit happened");

    co_return 42;
}

future<std::string> test_exception_3(std::string on_success) {
    try {
        auto coro = test_future_ex(
            invoke_on_destroy([]() { puts("[Calling invoke_on_destroy]"); }));
        auto val = co_await coro;
        co_return "No exception caught";
    } catch (std::logic_error& ex) {
        co_return on_success;
    }
}

RUN_CORO_TEST(test_exception_3);