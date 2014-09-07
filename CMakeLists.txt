cmake_minimum_required(VERSION 2.8.7)
project(PROJECT)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-std=c++1y -Wall -I./dependencies/lua/src -llua")

message(${CMAKE_SOURCE_DIR})

include_directories(include)

set(SOURCES
)

set(TESTS
    tests/tests.cpp
    tests/errors.cpp
    tests/moonscript.cpp
)

add_executable(ytest ${SOURCES} ${TESTS})
