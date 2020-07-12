#include <conduit/promise/future.hpp>
#include <conduit/unique_handle.hpp>

namespace conduit::async {
template <class T>
struct future : unique_handle<promise::future<T>> {
    using super = unique_handle<promise::future<T>>;
    using super::super;

    constexpr bool await_ready() noexcept { return super::done(); }
    constexpr auto await_suspend(std::coroutine_handle<> callback) noexcept {
        super::promise().set_callback(callback);
        return super::get_raw_handle();
    }
    constexpr auto await_resume() { return super::promise().get_value(); }
};
} // namespace conduit::async
