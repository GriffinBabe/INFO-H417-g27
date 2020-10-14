add_executable(
    example_test
    example_test.cpp
        ../include/stream/simple_input_stream.hpp ../include/stream/simple_output_stream.hpp)

target_link_libraries(example_test ${Boost_LIBRARIES} GTest::Main)
