#pragma once
#include <conduit/util/concepts.hpp>

namespace conduit::tags {
struct on_resume_t {
    explicit on_resume_t() = default;
};
constexpr auto on_resume = on_resume_t();

struct get_promise_t {
    explicit get_promise_t() = default;
};
constexpr auto get_promise = get_promise_t();

struct get_handle_t {
    explicit get_handle_t() = default;
};
constexpr auto get_handle = get_handle_t();

struct get_message_t {
    explicit get_message_t() = default;
};
constexpr auto get_message = get_message_t();

enum generator_mode : bool { check_first = false, resume_first = true };

struct nothing_t {
    using promise_type = std::noop_coroutine_promise;
    explicit nothing_t() = default;

    template <co_result coro>
    operator coro() const {
        co_return nothing_t();
    }
};
constexpr auto nothing = nothing_t();

} // namespace conduit::tags
