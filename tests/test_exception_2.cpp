#include "run_test.hpp"

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

RUN_CORO_TEST(test_exception_2)