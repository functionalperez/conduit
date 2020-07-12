#pragma once
#include <concepts.hpp>

struct get_promise_t { explicit get_promise_t() = default; };
constexpr auto get_promise = get_promise_t();

struct get_message_t { explicit get_message_t() = default; };
constexpr auto get_message = get_message_t();

enum generator_mode : bool { check_first = false, resume_first = true };

struct nothing_t {
    explicit nothing_t() = default;

    template<coroutine_type coro>
    operator coro() const {
        co_return nothing_t();
    }
};
constexpr auto nothing = nothing_t();
