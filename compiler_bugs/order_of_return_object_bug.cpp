#include <coroutine>
#include <iostream>

// See: https://stackoverflow.com/questions/62825103/should-the-coroutine-result-object-ever-be-constructed-after-initial-suspend

template <class Promise>
class coroutine {
    std::coroutine_handle<Promise> handle;

   public:
    using promise_type = Promise;
    coroutine(std::coroutine_handle<Promise> p) : handle(p) {
        std::cout << "  Running " << __PRETTY_FUNCTION__ << '\n';
    }
    coroutine(coroutine const&) = delete;
    coroutine(coroutine&&) = delete;

    std::coroutine_handle<Promise>* operator->() { return handle; }

    ~coroutine() {
        if (handle) {
            handle.destroy();
        }
    }
};
struct promise_base {
    std::suspend_never initial_suspend() {
        std::cout << "  Running " << __PRETTY_FUNCTION__ << '\n';
        return {};
    }
    std::suspend_always final_suspend() { return {}; }
    void return_void() {}
    void unhandled_exception() { std::terminate(); }
};

struct promise_A : promise_base {
    using handle = std::coroutine_handle<promise_A>;
    handle get_return_object() {
        std::cout << "  Running " << __PRETTY_FUNCTION__ << '\n';
        return handle::from_promise(*this);
    }
};
struct promise_B : promise_base {
    using handle = std::coroutine_handle<promise_B>;
    coroutine<promise_B> get_return_object() {
        std::cout << "  Running " << __PRETTY_FUNCTION__ << '\n';
        return {handle::from_promise(*this)};
    }
};
coroutine<promise_A> run_A() {
    std::cout << "Inside run_A()\n";
    co_return;
}
coroutine<promise_B> run_B() {
    std::cout << "Inside run_B()\n";
    co_return;
}

void test_promise_A() {
    std::cout << "------ Testing A ------\n";
    run_A();
    std::cout << "\n\n";
}
void test_promise_B() {
    std::cout << "------ Testing B ------\n";
    run_B();
    std::cout << "\n\n";
}
int main() {
    test_promise_A();
    test_promise_B();
}
