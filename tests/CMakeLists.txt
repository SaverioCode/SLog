cmake_minimum_required(VERSION 3.15)
project(SLogTests LANGUAGES CXX)

enable_testing()

set(EXE_NAME slog_tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable Address Sanitizer" ON)

find_package(GTest REQUIRED)

add_executable(${EXE_NAME} 
    src/log_levels.cpp
    src/mpsc_queue/single_thread_tests.cpp
    src/mpsc_queue/multi_thread_tests.cpp
    src/sinks/console_sink.cpp
    src/test_main.cpp
)

if (ENABLE_ASAN)
    if(MSVC)
        target_compile_options(${EXE_NAME} PRIVATE /fsanitize=address /Zi)
        target_link_options(${EXE_NAME} PRIVATE /INCREMENTAL:NO /DEBUG)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${EXE_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(${EXE_NAME} PRIVATE -fsanitize=address)
    endif()
endif()

target_link_libraries(${EXE_NAME} PRIVATE gtest::gtest slog::slog)

include(GoogleTest)
gtest_discover_tests(${EXE_NAME})
