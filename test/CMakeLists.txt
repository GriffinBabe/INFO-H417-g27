add_executable(
    test_stream
    stream/test_stream.cpp
)

target_link_libraries(test_stream ${Boost_LIBRARIES} GTest::Main)
