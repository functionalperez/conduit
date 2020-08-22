#pragma once
#include <conduit/util/iterator.hpp>
#include <conduit/util/concepts.hpp>
#include <conduit/util/stdlib_coroutine.hpp>

#include <utility>

namespace conduit {
template <class Promise>
struct unique_handle : private std::coroutine_handle<Promise> {
   private:
    // Used to have a reference to the Promise type in template expressions
    static Promise& declPromise();

   public:
    constexpr static bool is_return_object_aware =
        return_object_aware<Promise, unique_handle>;
    // Checks if the promise is noexcept
    constexpr static bool is_noexcept = Promise::is_noexcept;
    // This is used by the compiler to generate the coroutine frame
    using promise_type = Promise;
    // unique_handle inherits from super
    using super = std::coroutine_handle<Promise>;
    // Type of the coroutine handle
    using handle = std::coroutine_handle<Promise>;

    // true if the coroutine always suspends initially
    constexpr static bool suspends_initially =
        std::is_same_v<std::suspend_always,
                       decltype(declPromise().initial_suspend())>;

    // Returns true if the coroutine handle isn't null
    using super::operator bool;
    // Checks if the coroutine has finished executing
    using super::done;
    // Resumes execution of the coroutine
    using super::resume;
    // Obtains the promise object
    using super::promise;
    // Obtains the coroutine handle as a void*
    using super::address;

    // Creates a unique_handle from the promise_type object in the
    // coroutine_frame
    static unique_handle from_promise(promise_type& p) {
        return unique_handle(p);
    }
    // Creates a unique_handle from a void* representing a coroutine_handle
    static unique_handle from_address(void* addr) {
        return unique_handle(handle::from_address(addr));
    }

    // constructs a unique_handle from a coroutine handle
    unique_handle(std::coroutine_handle<Promise> h) : super(h) {
        if constexpr (is_return_object_aware) {
            promise().set_return_object(this);
        }
    }
    unique_handle(promise_type& h) : super(super::from_promise(h)) {
        if constexpr (is_return_object_aware) {
            h.set_return_object(this);
        }
    }
    unique_handle(unique_handle const&) = delete;
    unique_handle(unique_handle&& source) noexcept : super(source) {
        source.super::operator=(nullptr);
        if constexpr (is_return_object_aware) {
            promise().set_return_object(this);
        }
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
        super h = get();
        super::operator=(nullptr);
        if constexpr (is_return_object_aware) {
            h.promise().set_return_object(nullptr);
        }
        return h;
    }
    // destroys the coroutine and resets the coroutine_handle to null
    void reset(std::nullptr_t = nullptr) {
        if (*this) {
            super::destroy();
            super::operator=(nullptr);
        }
    }
    // Assigns another unique_handle without invoking destroy
    // on the current coroutine handle
    constexpr void assign_no_destroy(unique_handle other) noexcept {
        if constexpr (is_return_object_aware) {
            promise().set_return_object(nullptr);
        }
        super::operator=(other.get());
        other.super::operator=(nullptr);
        if constexpr (is_return_object_aware) {
            promise().set_return_object(this);
        }
    }
    constexpr void
    assign_no_destroy(std::coroutine_handle<Promise> other) noexcept {
        super::operator=(other);
        if constexpr (is_return_object_aware) {
            promise().set_return_object(this);
        }
    }
    // destroys the coroutine and replaces the coroutine handle with p
    void reset(std::coroutine_handle<Promise> p) {
        if (*this) {
            super::destroy();
            super::operator=(p);
        }
    }
    // Swaps this handle with another unique_handle
    void swap(unique_handle& other) noexcept {
        super tmp = *this;
        super::operator=(other);
        other.super::operator=(tmp);
        if constexpr (is_return_object_aware) {
            (*this)->set_return_object(this);
            other->set_return_object(&other);
        }
    }
    // Obtains a copy of the coroutine handle
    constexpr std::coroutine_handle<Promise> get() const noexcept {
        super tmp = *this;
        return tmp;
    }
    constexpr std::coroutine_handle<> get_raw_handle() const noexcept {
        return *this;
    }
    // Destroys the coroutine and resets the handle to null
    void destroy() { reset(); }
    ~unique_handle() {
        if (*this) {
            super::destroy();
        }
    }
    promise_type& operator*() noexcept { return super::promise(); }
    // Accesses the coroutine promise
    promise_type* operator->() noexcept { return &super::promise(); }
};
} // namespace conduit
