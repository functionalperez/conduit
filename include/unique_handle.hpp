#include <common.hpp>

template<class Promise>
struct unique_handle : private std::coroutine_handle<Promise> {
    using promise_type = Promise;
    using pointer = std::coroutine_handle<Promise>;

    // Returns true if the coroutine handle isn't null
    using pointer::operator bool;
    // Checks if the coroutine has finished executing
    using pointer::done;
    // Resumes execution of the coroutine
    using pointer::resume;
    // Obtains the promise object
    using pointer::promise;
    // Obtains the coroutine handle as a void*
    using pointer::address;

    // Creates a unique_handle from the promise_type object in the coroutine_frame
    static unique_handle from_promise(promise_type& p) {
        return unique_handle(pointer::from_promise(p)); 
    }
    // Creates a unique_handle from a void* representing a coroutine_handle
    static unique_handle from_address(void* addr) {
        return unique_handle(pointer::from_address(addr)); 
    }

    
    explicit unique_handle(pointer handle) : pointer(handle) {}
    unique_handle(unique_handle const&) = delete;
    unique_handle(unique_handle&& source) : pointer(source) {
        source.pointer::operator=(nullptr); 
    }

    unique_handle& operator=(std::nullptr_t) {
        reset(); 
        return *this;
    }
    unique_handle& operator=(unique_handle&& other) {
        // This implementation is nice b/c it handles the case of self-assignment
        auto tmp = std::move(other); 
        swap(tmp); 
    }

    pointer release() {
        pointer h = *this;
        *this = nullptr; 
        return h;
    }
    void reset(std::nullptr_t = nullptr) {
        if(*this) {
            pointer::destroy();
            pointer::operator=(nullptr);
        }
    }
    void reset(pointer p) {
        if(*this) {
            pointer::destroy();
            pointer::operator=(p);
        }
    }
    void swap(unique_handle& other) noexcept {
        pointer tmp = *this;
        pointer::operator=(other);
        other.pointer::operator=(tmp); 
    }
    pointer get() {
        return *this;
    }
    void destroy() {
        reset(); 
    }
    ~unique_handle() {
        if(*this) {
            pointer::destroy(); 
        }
    }
    // Accesses the promise_type
    promise_type* operator->() {
        return &pointer::promise(); 
    }
};