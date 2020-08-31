cmake_minimum_required(VERSION 3.0.0)
project(conduit VERSION 0.3.0 LANGUAGES CXX)

option(CONDUIT_Install "Install CMake targets during install step." ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
    add_compile_options("/std:c++latest" "/await" "/EHsc")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    add_compile_options("-std=c++20")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    add_compile_options("-std=c++20" "-fcoroutines")
endif()

include_directories(include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CONDUIT_TARGET_NAME ${PROJECT_NAME})
set(CONDUIT_INCLUDE_BUILD_DIR "${PROJECT_SOURCE_DIR}/include/")

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

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_executable(run_tests tests/tests.cpp)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(run_tests PRIVATE Threads::Threads)

    foreach(X IN ITEMS 
        test_coroutine
        test_destroy
        test_exception_1
        test_exception_2
        test_future
        test_generator
        test_on_suspend
        test_recursive_generator
        test_resume_on_alternate_thread
        test_source
        test_suspend_invoke
        test_task)
        add_test(NAME "${X}" COMMAND run_tests "${X}")
    endforeach()
endif()


