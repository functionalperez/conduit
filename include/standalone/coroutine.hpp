#include <coroutine>
#include <cstddef>
#include <memory>

template <class Promise>
struct coroutine_deleter {
    using pointer = std::coroutine_handle<Promise>;
    void operator()(std::coroutine_handle<Promise> handle) { handle.destroy(); }
};
class coroutine_promise;
class coroutine : private std::unique_ptr<void, coroutine_deleter<coroutine_promise>> {
    using base_type = std::unique_ptr<void, coroutine_deleter<coroutine_promise>>;
    base_type& as_base() noexcept { return *this; }

   public:
    using base_type::get;
    using promise_type = coroutine_promise;
    using handle = std::coroutine_handle<promise_type>;
    coroutine() = default;
    coroutine(coroutine const&) = delete;
    coroutine(coroutine&&) = default;
    explicit coroutine(std::nullptr_t) noexcept : base_type(nullptr) {}
    explicit coroutine(coroutine_promise& promise) noexcept
        : coroutine(handle::from_promise(promise)) {}
    explicit coroutine(handle h) noexcept : base_type(h) {}

    coroutine& operator=(coroutine other) {
        base_type::swap(other);
        return *this;
    }

    auto operator->() noexcept { return &get().promise(); }
    auto& operator*() noexcept { return get().promise(); }
    auto operator->() const noexcept { return &get().promise(); }
    auto& operator*() const noexcept { return get().promise(); }
    auto& promise() noexcept { return get().promise(); }
    auto& promise() const noexcept { return get().promise(); }
    bool done() const noexcept { return get().done(); }
    void resume() noexcept { get().resume(); }
};

class coroutine_promise {
   public:
    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() noexcept {}
    [[noreturn]] void unhandled_exception() noexcept { std::terminate(); }

    static coroutine get_return_object_on_allocation_failure() {
        return {};
    }
    coroutine get_return_object() { return coroutine(*this); }
};
