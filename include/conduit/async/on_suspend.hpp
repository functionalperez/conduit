#include <conduit/continuation.hpp>
#include <conduit/fn/bind.hpp>

#include <utility>

namespace conduit::async {

template <class Func, class Bind>
class on_suspend {
    struct package {
        [[no_unique_address]] Func func;
        [[no_unique_address]] Bind bind;
        std::coroutine_handle<> callback;
        void submit() { bind(func, callback); }
    };
    struct allocator {
        package p;
// A noop_continuation fits in 24 bytes in clang with optimization,
// But it requires 48 bytes in gcc, hence the difference
#ifdef __clang__
        alignas(void*) mutable char buffer[sizeof(void*) * 3]{};
#else
        alignas(void*) mutable char buffer[sizeof(void*) * 6]{};
#endif

        constexpr static size_t dynamic_offset = sizeof(package*);
        void* alloc(size_t size) {
            if (size <= sizeof(buffer)) {
                return buffer;
            } else {
                char* data = new char[dynamic_offset + size];
                new (data) package*(&p);
                return dynamic_offset + data;
            }
        }
        static void dealloc(void* addr, size_t size) {
            constexpr size_t static_offset = sizeof(package);
            if (size <= sizeof(buffer)) {
                package* p = (package*)((char*)addr - static_offset);
                p->submit();
            } else {
                package* p = *(package**)((char*)addr - dynamic_offset);
                p->submit();
                delete[]((char*)addr - dynamic_offset);
            }
        }
    };
    allocator alloc;

   public:
    template <class... Args>
    on_suspend(Func const& func, Args&&... args)
        : alloc{package{func, fn::bind_last(std::forward<Args>(args)...),
                        nullptr}} {}
    template <class... Args>
    on_suspend(Func&& func, Args&&... args)
        : alloc{package{std::move(func),
                        fn::bind_last(std::forward<Args>(args)...)}} {}

    constexpr bool await_ready() { return false; }
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> caller) {
        alloc.p.callback = caller;
        return noop_continuation(alloc);
    }
    void await_resume() {}
};

template <class F, class... Args>
on_suspend(F, Args... args) -> on_suspend<F, fn::bind_last_t<Args...>>;
} // namespace conduit::async
