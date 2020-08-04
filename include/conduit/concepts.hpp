#pragma once
#include <conduit/coroutine.hpp>
#include <cstddef>

namespace conduit {
template <class T>
concept non_void = !std::same_as<T, void>;

template <class T, class... Args>
using promise_t = typename std::coroutine_traits<T, Args...>::promise_type;

template <class T>
using handle_t = std::coroutine_handle<promise_t<T>>;

// clang-format off
template<class T, class... Alt>
concept same_as_either = (std::same_as<T, Alt> || ...);

template<class Awaitable>
concept hard_awaitable = requires(Awaitable a, std::coroutine_handle<> h) {
    { a.await_ready() } -> std::same_as<bool>;
    { a.await_suspend(h) } -> same_as_either<void, bool, std::coroutine_handle<>>;
    { a.await_resume() };
};

template<class Awaitable>
concept awaitable = hard_awaitable<Awaitable> || requires(Awaitable a) {
    { a.operator co_await() } -> hard_awaitable;
};

template<class Promise>
concept co_promise = requires(Promise p) {
    { p.initial_suspend() } -> awaitable;
    { p.final_suspend() } -> awaitable;
    { p.get_return_object() } -> non_void;
    { p.unhandled_exception() } -> std::same_as<void>;
};

template<class Result>
concept co_result = co_promise<promise_t<Result>> 
    && requires(promise_t<Result> p) {
    { Result{p.get_return_object()} } -> std::same_as<Result>;
};

template <class Promise, class Result>
concept return_object_aware = requires(Promise promise, Result* r) {
    { promise.set_return_object(r) };
};

template <class Promise>
concept value_producing_promise = co_promise<Promise> 
    && requires(Promise p) {
    { p.get_value() } -> non_void;
};

template <class Result, class Value>
concept can_co_return = co_result<Result> 
    && requires(promise_t<Result> p, Value v) {
    { p.return_value(v) } -> std::same_as<void>;
};
// clang-format on
} // namespace conduit
