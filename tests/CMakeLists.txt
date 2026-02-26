cmake_minimum_required(VERSION 3.15)
project(SLogTests LANGUAGES CXX)

enable_testing()

#########################
####  SHARED CONFIG  ####
#########################

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable Address Sanitizer" OFF)

find_package(GTest REQUIRED)

function(finalize_slog_test_target target_name)
    target_sources(${target_name} PRIVATE
        src/test_main.cpp
    )

    if (ENABLE_ASAN)
        if(MSVC)
            target_compile_options(${target_name} INTERFACE /fsanitize=address /Zi)
            target_link_options(${target_name} INTERFACE /INCREMENTAL:NO /DEBUG)
        elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target_name} INTERFACE -fsanitize=address -fno-omit-frame-pointer)
            target_link_options(${target_name} INTERFACE -fsanitize=address)
        endif()
    endif()

    target_link_libraries(${target_name} PRIVATE gtest::gtest slog::slog)

    include(GoogleTest)
    gtest_discover_tests(${target_name})
endfunction()

#######################
####  SYNC CONFIG  ####
#######################

function(add_slog_sync_tests target_name)
    add_executable(${target_name})

    target_sources(${target_name} PRIVATE
        src/mpsc_queue/single_thread_tests.cpp
        src/mpsc_queue/multi_thread_tests.cpp
        src/sinks/console_sink.cpp
        src/fmt/format_flags.cpp
        src/fmt/pattern_formatter.cpp
        src/fmt/deferred_format.cpp
    )

    finalize_slog_test_target(${target_name})
endfunction()

########################
####  ASYNC CONFIG  ####
########################

function(add_slog_async_tests target_name)
    add_executable(${target_name})

    target_sources(${target_name} PRIVATE
        src/async/async_test.cpp
    )

    target_compile_definitions(${target_name} PRIVATE SLOG_ASYNC_ENABLED)
    finalize_slog_test_target(${target_name})
endfunction()

#########################
#### ADD EXECUTABLES ####
#########################

add_slog_sync_tests(slog_tests_sync)
add_slog_async_tests(slog_tests_async)
