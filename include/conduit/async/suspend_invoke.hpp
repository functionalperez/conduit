#pragma once
#include <conduit/mixin/resumable.hpp>
#include <conduit/util/concepts.hpp>
#include <conduit/util/tag_types.hpp>

namespace conduit {
// clang-format off
template <class Func>
concept on_suspend_callback = requires(Func func, std::coroutine_handle<> h) {
    { func(h) } -> same_as<void>;
};

template <class Func>
concept on_resume_callback = requires(Func func) {
    { func(tags::on_resume) };
};
// clang-format on
} // namespace conduit

namespace conduit::async {
template <class Func>
struct suspend_invoke_container {
    [[no_unique_address]] Func on_suspend;
};

template <class Func>
struct suspend_invoke : suspend_invoke_container<Func>,
                        mixin::Resumable<suspend_invoke<Func>> {
    using suspend_invoke_container<Func>::on_suspend;
    using mixin::Resumable<suspend_invoke<Func>>::await_resume;
    decltype(auto) await_resume() {
        if constexpr (conduit::on_resume_callback<decltype(on_suspend)>) {
            return on_suspend(tags::on_resume);
        } else {
            return;
        }
    }
};

template <on_suspend_callback Func>
suspend_invoke(Func) -> suspend_invoke<Func>;
} // namespace conduit::async