file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/unit/*.cpp")
set(UNIT_TESTS ${PROJECT_NAME}_unit_tests)

add_executable(${UNIT_TESTS} ${SOURCES})
target_link_libraries(${UNIT_TESTS} ${PROJECT_NAME} GTest::gmock_main GTest::gtest)
