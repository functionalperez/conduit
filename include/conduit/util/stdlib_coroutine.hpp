#pragma once

#if defined(__GNUC__) && !defined(__clang__)
#if __GNUC__ == 10 && __GNUC_MINOR__ <= 2
#define CONDUIT_USE_GCC_EXCEPTION_WORKAROUND 1
#else
#warning "GCC versions 10, 10.1, and 10.2 contained a bug \
that resulted in the coroutine frame not being destroyed \
when an exception was thrown. Conduit implements a work-around \
for this bug that manually destroys the coroutine frame \
when an exception is thrown. This workaround is only enabled \
for these versions of GCC, so future versions of GCC that still \
contain this bug are not accounted for. Please upgrade to a newer \
version of conduit, or contact the maintainer at perez.cs@pm.me if \
a fix is needed for a newer version of GCC but with the same version of conduit."
#endif
#endif

#if __has_include(<coroutine>)
#if defined(__clang__) && !defined(__cpp_impl_coroutine)
#define __cpp_impl_coroutine 201902L
#endif
#include <coroutine>

#ifdef __clang__
    namespace std::experimental {
    using std::coroutine_handle;
    using std::coroutine_traits;
} // namespace std::experimental
#endif

#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
namespace std {
using std::experimental::coroutine_handle;
using std::experimental::coroutine_traits;
using std::experimental::noop_coroutine;
using std::experimental::noop_coroutine_handle;
using std::experimental::noop_coroutine_promise;
using std::experimental::suspend_always;
using std::experimental::suspend_never;
} // namespace std
#endif