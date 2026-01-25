cmake_minimum_required(VERSION 3.15)
project(LoggerTests LANGUAGES CXX)

enable_testing()

set(EXE_NAME slogger_tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable Address Sanitizer" ON)

if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

find_package(GTest REQUIRED)

add_executable(${EXE_NAME} 
    src/log_levels.cpp
    src/mpsc_queue/single_thread_tests.cpp
    src/mpsc_queue/multi_thread_tests.cpp
    src/test_main.cpp
)

target_link_libraries(${EXE_NAME} PRIVATE gtest::gtest slogger)

include(GoogleTest)
gtest_discover_tests(${EXE_NAME})
