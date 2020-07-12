# include/mini: a source of templates for writing coroutine types
The `include/mini` directory contains absolutely minimal implementations of a number of different types of coroutines, including generators, tasks, channels, and other assorted ideas. 

The point of these files is as a template meant to contain the bare minimum necessary in order to create a coroutine type with the given behavior. As a result of this, destructors, move constructors, and other quality-of-life features have been stripped out.

I strongly discourage the direct use of `include/mini` as a library. Instead, copy the files you need, and fill in the necessary quality-of-life features such as move constructors, destructors, and other functionality that improves the user experience when interacting with the coroutine. 
