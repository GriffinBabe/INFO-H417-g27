# Generate all the libraries in this folder

# Creates a new shared library - .dll windows / .so unix
# The library is also a "target" in cmake
add_library(
        streams SHARED
        stream/input_stream.cpp
        stream/output_stream.cpp
        stream/string_buffer.cpp
        stream/simple_input_stream.cpp
        stream/simple_output_stream.cpp
        stream/buffered_input_stream.cpp
        stream/buffered_output_stream.cpp
        stream/mmap_input_stream.cpp
        stream/mmap_output_stream.cpp
        stream/input_stdio_stream.cpp
        stream/ouput_stdio_stream.cpp
)

target_link_libraries(streams PUBLIC ${Boost_LIBRARIES}) # links boost libraries

add_executable(
        length
        performance/length_main.cpp
)

target_link_libraries(length PUBLIC streams)

add_executable(
        randjump
        performance/randjump_main.cpp
)

target_link_libraries(randjump PUBLIC streams)

set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/database)
set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/output)
configure_file(resources_config.hpp.in ${CMAKE_SOURCE_DIR}/include/resources_config.hpp)
