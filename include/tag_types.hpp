// This provides boilerplate to make a minimal tag_type
// With the ability to prevent ambiguity in overload resolution
template<class tag>
struct tag_type {
    tag_type(tag_type const&) = default;
    constexpr static tag make() {
        return tag();
    }
    // Making tag_type explicit prevents ambiguity in overload resolution
    explicit tag_type() = default;
};

struct get_promise_t : tag_type<get_promise_t> {};
constexpr get_promise_t get_promise = get_promise_t();

struct get_message_t : tag_type<get_message_t> {};
constexpr get_message_t get_message = get_message_t();
