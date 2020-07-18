#pragma once
#include <coroutine>

namespace conduit {
template <class T>
using promise_t = typename std::coroutine_traits<T>::promise_type;

template <class T>
using handle_t = std::coroutine_handle<promise_t<T>>;

template <class Promise, class ReturnObject>
concept return_object_aware = requires(Promise promise, ReturnObject* r) {
    {promise.set_return_object(r)};
};
template <class Promise>
concept value_producing_promise = requires(Promise p) {
    { p.get_value() };
};

template <class Coro>
concept coroutine_type = requires(handle_t<Coro> handle) {
    {Coro(handle)};
};

template <class Coro, class type>
concept can_co_return = coroutine_type<Coro>&& requires(promise_t<Coro> promise, type t) {
    promise.return_value(t);
};
} // namespace conduit
