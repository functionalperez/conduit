#pragma once
#include <common.hpp>
#include <iterator.hpp>

template <class Promise>
struct unique_handle : private std::coroutine_handle<Promise> {
   private:
    // Used to have a reference to the Promise type in template expressions
    static Promise& declPromise();

   public:
    constexpr static bool is_return_object_aware = return_object_aware<Promise, unique_handle>;
    // Checks if the promise is noexcept
    constexpr static bool is_noexcept = Promise::is_noexcept;
    // This is used by the compiler to generate the coroutine frame
    using promise_type = Promise;
    // unique_handle inherits from base_type
    using base_type = std::coroutine_handle<Promise>;
    // Type of the coroutine handle
    using handle = std::coroutine_handle<Promise>;

    // true if the coroutine always suspends initially
    constexpr static bool suspends_initially =
        std::is_same_v<std::suspend_always,
                       decltype(declPromise().initial_suspend())>;

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
    explicit unique_handle(std::coroutine_handle<Promise> h) : base_type(h) {
        if constexpr(is_return_object_aware) {
            h.promise().set_return_object(this);
        }
    }
    explicit unique_handle(Promise& h) : base_type(base_type::from_promise(h)) {
        if constexpr(is_return_object_aware) {
            h.set_return_object(this);
        }
    }
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
    // This is reset to null but the coroutine is NOT destroyed
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
    std::coroutine_handle<Promise> get() noexcept {
        return *static_cast<base_type*>(this);
    }
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

template <class T>
auto begin(unique_handle<T>& p) {
    // If the coroutine handle always suspends initially, we need to resume it
    // when we begin
    if constexpr (unique_handle<T>::suspends_initially) {
        p.resume();
    }
    return coro_iterator{p.get()};
}
template <class T>
auto end(unique_handle<T>& p) {
    // coro_sentinal is just used to proivide an overload for
    // coro_iterator::operator== so it doesn't need to contain any information
    // about the coroutine handle
    return coro_sentinal{};
}
