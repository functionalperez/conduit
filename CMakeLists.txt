cmake_minimum_required(VERSION 3.0.0)
project(conduit VERSION 0.1.0 LANGUAGES CXX)

option(CONDUIT_Install "Install CMake targets during install step." ON)

add_compile_options("-std=c++20")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  add_compile_options("-fcoroutines")
endif()

include_directories(include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CONDUIT_TARGET_NAME ${PROJECT_NAME})
set(CONDUIT_INCLUDE_BUILD_DIR "${PROJECT_SOURCE_DIR}/include/")

add_executable(run_tests main.cpp)

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
