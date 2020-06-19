cmake_minimum_required(VERSION 3.10)

# set the project name
project(conduit)

set(CMAKE_C_COMPILER "gcc-10")
set(CMAKE_CXX_COMPILER "g++-10")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options("-fcoroutines")
include_directories(include)

# add the executable
add_executable(run_example example.cpp)
