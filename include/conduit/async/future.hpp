#include <conduit/promise/future.hpp>
#include <conduit/unique_handle.hpp>

namespace async {
template <class T>
struct future : unique_handle<promise::future<T>> {
    using super = unique_handle<promise::future<T>>;
    using super::super;

    bool await_ready() noexcept { return super::done(); }
    auto await_suspend(std::coroutine_handle<> callback) noexcept {
        super::promise().set_callback(callback);
        return super::get_raw_handle();
    }
    auto await_resume() { return super::promise().get_value(); }
};
} // namespace async
