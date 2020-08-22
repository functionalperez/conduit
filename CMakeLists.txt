cmake_minimum_required(VERSION 3.0.0)
project(conduit VERSION 0.1.0)

add_compile_options("-std=c++20")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  add_compile_options("-fcoroutines")
endif()

include_directories(include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(conduit main.cpp)
