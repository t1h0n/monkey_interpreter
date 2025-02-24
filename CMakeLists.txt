cmake_minimum_required(VERSION 3.10)
project(
    monkey_compiler
    LANGUAGES CXX
    VERSION 1.0)
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT output)
if(supported)
    message(STATUS "IPO / LTO enabled for Release and RelWithDebInfo builds")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:TRUE>)
else()
    message(STATUS "IPO / LTO not supported: ${output}")
endif()

option(${PROJECT_NAME}_ENABLE_PARSE_TRACING "Specify if parsing tracing should be active" OFF)
option(${PROJECT_NAME}_ENABLE_TESTING "Specify if parsing testing should be enabled" ON)

include(FetchContent)
FetchContent_Declare(
    range-v3
    GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
    GIT_TAG 0.12.0)
FetchContent_MakeAvailable(range-v3)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.1.3)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.7)
FetchContent_MakeAvailable(magic_enum)

if(${PROJECT_NAME}_ENABLE_TESTING)
    set(gtest_force_shared_crt
        ON
        CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2)
    FetchContent_MakeAvailable(googletest)
endif()
FetchContent_Declare(
    find_fs
    GIT_REPOSITORY https://github.com/t1h0n/cpp_find_fs.git
    GIT_TAG main)
FetchContent_MakeAvailable(find_fs)

add_subdirectory(lib)
add_subdirectory(apps)

if(${PROJECT_NAME}_ENABLE_TESTING)
    add_subdirectory(test)
endif()
