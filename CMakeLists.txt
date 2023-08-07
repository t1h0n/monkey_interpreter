cmake_minimum_required(VERSION 3.10)
project(monkey_compiler LANGUAGES CXX)

find_package(fmt REQUIRED)
find_package(GTest REQUIRED)
find_package(magic_enum REQUIRED)
find_package(range-v3 REQUIRED)

option(${PROJECT_NAME}_ENABLE_PARSE_TRACING
       "Specify if parsing tracing should be active" OFF)
add_subdirectory(dep/fs)

add_subdirectory(lib)
add_subdirectory(apps)
add_subdirectory(test)
