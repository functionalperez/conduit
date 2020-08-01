#include <conduit/continuation.hpp>
#include <conduit/fn/bind.hpp>

#include <utility>

namespace conduit::async {

template <class F, class Bind>
class on_suspend {
    struct package {
        [[no_unique_address]] F func;
        [[no_unique_address]] Bind bind;
        std::coroutine_handle<> handle = nullptr;
        void operator()() { bind(func, handle); }
    };

    using allocator = mem::linda<package, noop_continuation_size>;

    allocator alloc;

   public:
    template <class... Args>
    on_suspend(F const& func, Args&&... args)
      : alloc{{}, package{func, fn::bind_last(std::forward<Args>(args)...)}} {}
    template <class... Args>
    on_suspend(F&& func, Args&&... args)
      : alloc{{},
              package{std::move(func),
                      fn::bind_last(std::forward<Args>(args)...)}} {}

    constexpr bool await_ready() { return false; }
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> caller) {
        alloc.callback.handle = caller;
        return noop_continuation(alloc);
    }
    void await_resume() {}
};

template <class F, class... Args>
on_suspend(F, Args... args) -> on_suspend<F, fn::bind_last_t<Args...>>;

} // namespace conduit::async