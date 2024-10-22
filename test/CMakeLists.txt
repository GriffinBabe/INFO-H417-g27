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

add_executable(
    buffered_stream_test
    stream/buffered_stream_test.cpp
)
target_link_libraries(buffered_stream_test streams GTest::Main)

add_executable(
    mmap_stream_test
    stream/mmap_stream_test.cpp
)
target_link_libraries(mmap_stream_test streams GTest::Main)

add_executable(
    stdio_test
    stream/stream_stdio_test.cpp
)
target_link_libraries(stdio_test streams GTest::Main)

gtest_discover_tests(example_test)
gtest_discover_tests(simple_stream_test)
gtest_discover_tests(buffered_stream_test)
gtest_discover_tests(mmap_stream_test)
gtest_discover_tests(stdio_test)
