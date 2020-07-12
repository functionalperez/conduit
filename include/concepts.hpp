#pragma once
#include <coroutine>

template<class Promise, class ReturnObject>
concept return_object_aware = requires(Promise promise, ReturnObject* r) {
    { promise.set_return_object(r) };
};

template<class Coro>
concept coroutine_type = requires(typename std::coroutine_traits<Coro>::promise_type promise) {
    { Coro(promise) };
};
