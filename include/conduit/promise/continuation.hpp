#pragma once
#include <conduit/mem/allocator.hpp>
#include <conduit/mixin/promise_parts.hpp>

namespace conduit::promise {
template <mem::allocator Alloc>
struct continuation : mixin::GetReturnObject<continuation<Alloc>, false>,
                      mixin::UnhandledException<true>,
                      mixin::InitialSuspend<mixin::always>,
                      mixin::FinalSuspend<mixin::never>,
                      mixin::ReturnVoid,
                      mixin::NewAndDelete<Alloc> {};
} // namespace conduit::promise
