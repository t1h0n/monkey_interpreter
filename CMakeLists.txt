cmake_minimum_required(VERSION 3.10)
project(monkey_compiler LANGUAGES CXX)

find_package(fmt REQUIRED)
find_package(GTest REQUIRED)
find_package(magic_enum REQUIRED)
find_package(range-v3 REQUIRED)

add_subdirectory(dep/fs)

add_subdirectory(lib)
add_subdirectory(apps)
add_subdirectory(test)
