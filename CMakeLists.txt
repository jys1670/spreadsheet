cmake_minimum_required(VERSION 3.24)

SET(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(spreadsheet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        unit-tests
        src/structures.cpp
        src/common.h
        tests/main.cpp
        tests/test_runner_p.h
)