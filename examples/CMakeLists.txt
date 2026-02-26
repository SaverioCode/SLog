cmake_minimum_required(VERSION 3.15)
project(SLogExamples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

###########################
####  EXAMPLE OPTIONS  ####
###########################

option(SLOG_BUILD_ALL_EXAMPLES       "Build all examples"               ON)

option(SLOG_EXAMPLE_BASIC_LOGGING    "Build basic_logging example"      OFF)
option(SLOG_EXAMPLE_STREAM_LOGGING   "Build stream_logging example"     OFF)
option(SLOG_EXAMPLE_MULTI_SINK       "Build multi_sink example"         OFF)
option(SLOG_EXAMPLE_FILE_SINK        "Build file_sink example"          OFF)
option(SLOG_EXAMPLE_CONSOLE_SINK     "Build console_sink example"       OFF)
option(SLOG_EXAMPLE_CUSTOM_FORMATTER "Build custom_formatter example"   OFF)
option(SLOG_EXAMPLE_LOG_LEVELS       "Build log_levels example"         OFF)

#############################
####  HELPER FUNCTION    ####
#############################

function(slog_add_example EXAMPLE_NAME)
    cmake_parse_arguments(ARG "" "" "DEFINITIONS" ${ARGN})

    if(NOT SLOG_BUILD_ALL_EXAMPLES AND NOT SLOG_EXAMPLE_${EXAMPLE_NAME})
        return()
    endif()

    string(TOLOWER ${EXAMPLE_NAME} dir_name)
    set(target_name slog_example_${dir_name})

    add_executable(${target_name} ${dir_name}/main.cpp)
    target_link_libraries(${target_name} PRIVATE slog::slog)

    target_compile_definitions(${target_name} PRIVATE SLOG_HEADER_ONLY)

    if(ARG_DEFINITIONS)
        target_compile_definitions(${target_name} PRIVATE ${ARG_DEFINITIONS})
    endif()

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Werror)
    endif()
endfunction()

##############################
####  REGISTER EXAMPLES   ####
##############################

slog_add_example(MULTI_SINK)
slog_add_example(FILE_SINK)
slog_add_example(CONSOLE_SINK)
slog_add_example(SET_PATTERN)
slog_add_example(LOG_LEVELS)
slog_add_example(BASIC_LOGGING DEFINITIONS SLOG_STREAM_ENABLED)
