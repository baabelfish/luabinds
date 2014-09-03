cmake_minimum_required(VERSION 3.0)
project(PROJECT)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-std=c++1y -Wall -llua")

message(${CMAKE_SOURCE_DIR})

include_directories(include)

set(SOURCES
)

set(TESTS
    tests/tests.cpp
)

add_executable(ytest ${SOURCES} ${TESTS})
