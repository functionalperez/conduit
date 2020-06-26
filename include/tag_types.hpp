struct get_promise_t {
    get_promise_t(get_promise_t const&) = default;
    constexpr static get_promise_t make() { return get_promise_t(); }

   private:
    // By making this constructor private, we avoid ambiguity
    // in cases of co_yield {};
    get_promise_t() = default;
};
constexpr get_promise_t get_promise = get_promise_t::make();
