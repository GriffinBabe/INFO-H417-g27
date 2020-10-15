add_executable(
    example_test
    example_test.cpp
)
target_link_libraries(example_test ${Boost_LIBRARIES} GTest::Main)

add_executable(
    simple_stream_test
    stream/simple_stream_test.cpp
)
target_link_libraries(simple_stream_test streams GTest::Main)

gtest_discover_tests(example_test)
gtest_discover_tests(simple_stream_test)
