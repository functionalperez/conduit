#include <common.hpp>

template <class Promise>
struct unique_handle : private std::coroutine_handle<Promise> {
    constexpr static bool is_noexcept = Promise::is_noexcept;
    using promise_type = Promise;
    using base_type = std::coroutine_handle<Promise>;
    using handle = std::coroutine_handle<Promise>;

    // Returns true if the coroutine handle isn't null
    using base_type::operator bool;
    // Checks if the coroutine has finished executing
    using base_type::done;
    // Resumes execution of the coroutine
    using base_type::resume;
    // Obtains the promise object
    using base_type::promise;
    // Obtains the coroutine handle as a void*
    using base_type::address;

    // Creates a unique_handle from the promise_type object in the
    // coroutine_frame
    static unique_handle from_promise(promise_type& p) {
        return unique_handle(handle::from_promise(p));
    }
    // Creates a unique_handle from a void* representing a coroutine_handle
    static unique_handle from_address(void* addr) {
        return unique_handle(handle::from_address(addr));
    }

    // constructs a unique_handle from a coroutine handle
    explicit unique_handle(std::coroutine_handle<Promise> h) : base_type(h) {}
    unique_handle(unique_handle const&) = delete;
    unique_handle(unique_handle&& source) : base_type(source) {
        source.base_type::operator=(nullptr);
    }

    // destroys the coroutine and resets the coroutine_handle to null
    unique_handle& operator=(std::nullptr_t) {
        reset();
        return *this;
    }
    // Implements move assignment operator efficently
    unique_handle& operator=(unique_handle other) {
        // This implementation is nice b/c it handles the case of
        // self-assignment
        swap(other);
        return *this;
    }
    // Releases ownership of the coroutine handle and returns the handle
    std::coroutine_handle<Promise> release() noexcept {
        base_type h = get();
        base_type::operator=(nullptr);
        return h;
    }
    // destroys the coroutine and resets the coroutine_handle to null
    void reset(std::nullptr_t = nullptr) {
        if (*this) {
            base_type::destroy();
            base_type::operator=(nullptr);
        }
    }
    // destroys the coroutine and replaces the coroutine handle with p
    void reset(std::coroutine_handle<Promise> p) {
        if (*this) {
            base_type::destroy();
            base_type::operator=(p);
        }
    }
    // Swaps this handle with another unique_handle
    void swap(unique_handle& other) noexcept {
        base_type tmp = *this;
        base_type::operator=(other);
        other.base_type::operator=(tmp);
    }
    // Obtains a copy of the coroutine handle
    std::coroutine_handle<Promise> get() noexcept { return *static_cast<base_type>(this); }
    // Destroys the coroutine and resets the handle to null
    void destroy() { reset(); }
    ~unique_handle() {
        if (*this) {
            base_type::destroy();
        }
    }
    // Accesses the coroutine promise
    promise_type* operator->() noexcept { return &base_type::promise(); }
};