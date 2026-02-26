cmake_minimum_required(VERSION 3.15)
project(SLog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_NAME slog)

###################
####  OPTIONS  ####
###################

option(SLOG_BUILD_TESTS "Build with tests" OFF)
option(SLOG_BUILD_EXAMPLES "Build examples" OFF)

set(BUILD_OPTIONS "STATIC" "SHARED" "HEADER_ONLY")
set(SLOG_BUILD_TYPE "STATIC" CACHE STRING "SLOG_BUILD_TYPE chosen by the user at CMake configure time")
set_property(CACHE SLOG_BUILD_TYPE PROPERTY STRINGS ${BUILD_OPTIONS})

if(NOT SLOG_BUILD_TYPE IN_LIST BUILD_OPTIONS)
    message(FATAL_ERROR "Missing or Invalid SLOG_BUILD_TYPE. Available options are: ${BUILD_OPTIONS}")
endif()

##########################
####  TARGET LIBRARY  ####
##########################

if (${SLOG_BUILD_TYPE} STREQUAL "HEADER_ONLY")
    add_library(${TARGET_NAME} INTERFACE)
    target_include_directories(${TARGET_NAME} INTERFACE include)
elseif (${SLOG_BUILD_TYPE} STREQUAL "STATIC")
    add_library(${TARGET_NAME} STATIC)
    target_include_directories(${TARGET_NAME} PUBLIC include)
elseif (${SLOG_BUILD_TYPE} STREQUAL "SHARED")
    add_library(${TARGET_NAME} SHARED)
    target_include_directories(${TARGET_NAME} PUBLIC include)
endif()


if (NOT ${SLOG_BUILD_TYPE} STREQUAL "HEADER_ONLY")
    target_sources(${TARGET_NAME} PRIVATE src/slog_lib.cpp)
endif()

############################
####  COMPILER OPTIONS  ####
############################

if(${SLOG_BUILD_TYPE} STREQUAL "HEADER_ONLY")
    set(OPTS_SCOPE INTERFACE)
else()
    set(OPTS_SCOPE PRIVATE)
endif()

target_compile_definitions(slog ${OPTS_SCOPE} 
    $<$<STREQUAL:${SLOG_BUILD_TYPE},HEADER_ONLY>:SLOG_HEADER_ONLY>
)

if(MSVC)
    target_compile_options(${TARGET_NAME} ${OPTS_SCOPE} /W4 /WX)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${TARGET_NAME} ${OPTS_SCOPE} -Wall -Wextra -Werror)
endif()

##################
####  ALIAS  #####
##################

add_library(slog::slog ALIAS slog)

#################
####  TESTS  ####
#################

if(SLOG_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(SLOG_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
