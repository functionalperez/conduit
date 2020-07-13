The gcc provided by gcc-10 package produces internal compiler errors not present in gcc version 10.1

BACKGROUND

Simple code involving coroutines results in internal compiler errors in the version of gcc provided by this package. I am filing a bug report here because gcc version 10.1 does not produce these errors, and these errors are only present in the compiler version provided by the gcc-10 package.

On my system, this version is:

 g++-10 (Ubuntu 10-20200411-0ubuntu1) 10.0.1 20200411 (experimental) [master revision bb87d5cc77d:75961caccb7:f883c46b4877f637e0fa5025b4d6b5c9040ec566]

REPRODUCING THE BUG

In order to reproduce the bug, create a file main.cpp and compile as:

g++-10 -std=c++20 -fcoroutines main.cpp

Compiling this file will produce an internal compiler error. Here, g++-10 invokes the g++ compiler provided by this package. This file should contain the following code (code inside ticks):
```
#include <coroutine>
#include <exception>

// Compiler version resulting in bug: gcc 10.0.1 20200411 (experimental) 
// Compile with:
// g++-10 -std=c++20 -fcoroutines main.cpp

struct promise {
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() { return {}; }
    void return_void() {}
    bool get_return_object() { return true; }
    static bool get_return_object_on_allocation_failure() { return false; }
    [[noreturn]] void unhandled_exception() { std::terminate(); }
};

struct result {
    bool success;
    result(bool success) : success(success) {}
    using promise_type = promise;
};

result foo() {
    co_return; 
}

int main() {
    foo();
}
```

Under the version of gcc provided by this package, the output of compilation is:

during RTL pass: expand
main.cpp: In function ‘result foo()’:
main.cpp:25:1: internal compiler error: in expand_expr_addr_expr_1, at expr.c:8070
   25 | }
      | ^
Please submit a full bug report,
with preprocessed source if appropriate.
See <file:///usr/share/doc/gcc-10/README.Bugs> for instructions.

Internal compiler errors such as these were fixed in gcc 10.1. Here is a link to Compiler Explorer demonstrating that it compiles fine under 10.1: https://godbolt.org/z/dMrY8r

There are many similar code examples that result in internal compiler errors, and I believe the occurrence of these errors will be frustrating for anyone attempting to use C++20 features such as coroutines. In addition, some code examples (such as the one above) only produce a compiler error when there's no optimization flag, but compile fine with optimization enabled. This is extremely unexpected behavior. 

