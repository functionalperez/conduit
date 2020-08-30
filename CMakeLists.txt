cmake_minimum_required(VERSION 3.0.0)
project(conduit VERSION 0.3.0 LANGUAGES CXX)

option(CONDUIT_Install "Install CMake targets during install step." ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
    add_compile_options("/std:c++latest" "/await")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    add_compile_options("-std=c++20")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
    add_compile_options("-std=c++20" "-fcoroutines")
endif()

include_directories(include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CONDUIT_TARGET_NAME ${PROJECT_NAME})
set(CONDUIT_INCLUDE_BUILD_DIR "${PROJECT_SOURCE_DIR}/include/")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(run_tests main.cpp)

target_link_libraries(run_tests PRIVATE Threads::Threads)



add_library(${CONDUIT_TARGET_NAME} INTERFACE)
 
target_compile_definitions(
    ${CONDUIT_TARGET_NAME}
    INTERFACE
)

target_include_directories(
    ${CONDUIT_TARGET_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${CONDUIT_INCLUDE_BUILD_DIR}>
    $<INSTALL_INTERFACE:include>
)

install(
    DIRECTORY ${CONDUIT_INCLUDE_BUILD_DIR}
    DESTINATION include
)
