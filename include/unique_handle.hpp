#include <common.hpp>

template<class Promise>
struct unique_handle : private std::coroutine_handle<Promise> {
    using promise_type = Promise;
    using handle_type = std::coroutine_handle<Promise>;
    using handle_type::operator bool;
    using handle_type::done;
    using handle_type::resume;
    using handle_type::promise;
    using handle_type::from_promise;
    using handle_type::from_address;
    
    
    explicit unique_handle(handle_type handle) : handle_type(handle) {}
    unique_handle(unique_handle const&) = delete;
    unique_handle(unique_handle&& source) : handle_type(source) {
        source.handle_type::operator=(nullptr); 
    }

    unique_handle& operator=(std::nullptr_t) {
        if(*this) {
            handle_type::destroy();
            handle_type::operator=(nullptr);
        }
        return *this;
    }
    unique_handle& operator=(unique_handle&& other) {
        // This implementation is nice b/c it handles the case of self-assignment
        auto tmp = std::move(other); 
        swap(tmp); 
    }

    handle_type detach() {
        handle_type h = *this;

    }
    void swap(unique_handle& other) noexcept {
        handle_type tmp = *this;
        handle_type::operator=(other);
        other.handle_type::operator=(tmp); 
    }
    void destroy() {
        *this = nullptr; 
    }
    ~unique_handle() {
        if(*this) {
            handle_type::destroy(); 
        }
    }
};