cmake_minimum_required(VERSION 3.0.0)
project(conduit VERSION 0.3.0 LANGUAGES CXX)

add_library(conduit INTERFACE)

option(CONDUIT_Install "Install CMake targets during install step." ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
    target_compile_options(conduit INTERFACE "/std:c++latest" "/await" "/EHsc")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    target_Compile_options(conduit INTERFACE "-std=c++20")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    target_compile_options(conduit INTERFACE "-std=c++20" "-fcoroutines")
endif()

target_compile_definitions(conduit INTERFACE)

target_include_directories(
    conduit
    INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:include>
)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_executable(run_tests tests/tests.cpp)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(run_tests PRIVATE conduit)
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


