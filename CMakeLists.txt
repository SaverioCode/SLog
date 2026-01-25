cmake_minimum_required(VERSION 3.15)
project(SimpleLogger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(slogger INTERFACE)
target_include_directories(slogger INTERFACE include)

enable_testing()

add_subdirectory(tests)
