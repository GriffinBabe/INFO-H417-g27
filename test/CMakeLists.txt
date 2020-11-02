add_executable(
    example_test
    example_test.cpp
)
target_link_libraries(example_test ${Boost_LIBRARIES} GTest::Main)

add_executable(
    mmap_stream_test
    stream/mmap_stream_test.cpp
)
target_link_libraries(mmap_stream_test streams GTest::Main)

gtest_discover_tests(example_test)
gtest_discover_tests(mmap_stream_test)