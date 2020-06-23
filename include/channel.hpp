#pragma once
#include <channel_promise.hpp>
#include <unique_handle.hpp>

template <class Promise>
struct unique_channel : public unique_handle<Promise> {
    using message_type = typename Promise::message_type;
    using base = unique_handle<Promise>;
    using base::base;

    void send(message_type const& message) {
        base::promise().message = message;
    }
};

template <class T, class Message = T, bool is_noexcept = true,
          bool generator_mode = check_first>
using channel = unique_channel<
    channel_promise<T, Message, unique_channel, is_noexcept, generator_mode>>;

template <class T, class Message, bool is_noexcept, bool which_first>
auto& operator<<(channel<T, Message, is_noexcept, which_first>& ch,
                 Message const& m) noexcept(is_noexcept) {
    ch.send(m);
    ch.resume();
    return ch;
}
template <class T, class Message, bool is_noexcept>
bool operator>>(channel<T, Message, is_noexcept, check_first>& g,
                T& value) noexcept(is_noexcept) {
    if (g.done())
        return false;
    value = g->value;
    return true;
}
template <class T, class Message, bool is_noexcept>
bool operator>>(channel<T, Message, is_noexcept, resume_first>& g,
                T& value) noexcept(is_noexcept) {
    if (g.done())
        return false;
    value = g->value;
    return true;
}