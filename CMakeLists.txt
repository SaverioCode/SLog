cmake_minimum_required(VERSION 3.15)
project(SLog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

set(TARGET_NAME slog)

###################
####  OPTIONS  ####
###################

option(SLOG_BUILD_TESTS "Build with tests" OFF)
option(SLOG_BUILD_EXAMPLES "Build examples" OFF)
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(SLOG_ASYNC_ENABLED "Enable Asynchronous logic" OFF)

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
    set(OPTS_SCOPE PUBLIC)
endif()

target_compile_definitions(slog ${OPTS_SCOPE} 
    $<$<STREQUAL:${SLOG_BUILD_TYPE},HEADER_ONLY>:SLOG_HEADER_ONLY>
    $<$<STREQUAL:${SLOG_BUILD_TYPE},SHARED>:SLOG_SHARED_LIB>
    $<$<BOOL:${SLOG_ASYNC_ENABLED}>:SLOG_ASYNC_ENABLED>
)

if(MSVC)
    # /wd4251: Disable "class template ... needs to have dll-interface to be used by clients of class ..."
    target_compile_options(${TARGET_NAME} ${OPTS_SCOPE} /W4 /WX /wd4251)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${TARGET_NAME} ${OPTS_SCOPE} -Wall -Wextra -Werror)
endif()

#########################
####  SANITIZERS     ####
#########################

if(${SLOG_BUILD_TYPE} STREQUAL "HEADER_ONLY")
    set(ASAN_OPTS_SCOPE INTERFACE)
else()
    set(ASAN_OPTS_SCOPE PUBLIC)
endif()

if(ENABLE_ASAN)
    if(MSVC)
        target_compile_options(${TARGET_NAME} ${ASAN_OPTS_SCOPE} /fsanitize=address /Zi)
        target_link_options(${TARGET_NAME} ${ASAN_OPTS_SCOPE} /INCREMENTAL:NO /DEBUG)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${TARGET_NAME} ${ASAN_OPTS_SCOPE} -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(${TARGET_NAME} ${ASAN_OPTS_SCOPE} -fsanitize=address)
    endif()

    # To avoid MSVC error linking Gtest(that is not built with ASAN) when the library is built with ASAN
    if(MSVC AND ENABLE_ASAN)
        target_compile_definitions(${TARGET_NAME} ${ASAN_OPTS_SCOPE} 
            _DISABLE_STRING_ANNOTATION=1
            _DISABLE_VECTOR_ANNOTATION=1
        )
    endif()
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

#########################
####  INSTALLATION   ####
#########################

include(GNUInstallDirs)

install(TARGETS ${TARGET_NAME}
    EXPORT SLogTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
