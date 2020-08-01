#pragma once
#include <utility>

namespace conduit::fn {
template <class T>
T decl();

using std::forward;

template <class T>
struct arg {
    [[no_unique_address]] T elem;
    T&& fwd() { return std::move(elem); }
};
template <class T>
struct arg<T&> {
    T& elem;
    T& fwd() { return elem; }
};
template <class T>
struct arg<T&&> {
    [[no_unique_address]] T elem;
    T&& fwd() { return std::move(elem); }
};
template <class T>
arg(T) -> arg<T>;
template <class T>
arg(T&) -> arg<T&>;

template <class... T>
auto bind_last(T&&... last) {
    return
        [... last_ = arg{forward<T>(last)}](auto&& f, auto&&... first) mutable {
            return forward<decltype(f)>(f)(forward<decltype(first)>(first)...,
                                           last_.fwd()...);
        };
}
template <class... T>
auto bind_first(T&&... first) {
    return [... first_ = arg{forward<T>(first)}](auto&& f,
                                                 auto&&... last) mutable {
        return forward<decltype(f)>(f)(first_.fwd()...,
                                       forward<decltype(last)>(last)...);
    };
}

template <class... T>
using bind_first_t = decltype(bind_first(decl<T>()...));

template <class... T>
using bind_last_t = decltype(bind_last(decl<T>()...));
} // namespace conduit::fn