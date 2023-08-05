file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt magic_enum::magic_enum
                                             range-v3::range-v3 fs_bindings)
target_include_directories(${PROJECT_NAME} PUBLIC include)
