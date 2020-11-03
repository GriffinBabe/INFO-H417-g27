add_executable(
    example_test
    example_test.cpp
)
add_executable(
        stdio_test
        stream/stream_stdio_test.cpp
)

target_link_libraries(example_test ${Boost_LIBRARIES} GTest::Main)
target_link_libraries(stdio_test streams GTest::Main)
gtest_discover_tests(stdio_test)