cmake_minimum_required(VERSION 3.0.0)
project(conduit CXX)

add_library(conduit INTERFACE)

option(CONDUIT_Install "Install CMake targets during install step." ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
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
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    
    file(GLOB all_tests "tests/*.cpp")

    foreach(file_name ${all_tests})
        get_filename_component(test_name ${file_name} NAME_WE)
        add_executable("${test_name}" "${file_name}")
        target_link_libraries("${test_name}" PRIVATE conduit)
        target_link_libraries("${test_name}" PRIVATE Threads::Threads)
        add_test(NAME "${test_name}" COMMAND "${test_name}")
    endforeach()
endif()


