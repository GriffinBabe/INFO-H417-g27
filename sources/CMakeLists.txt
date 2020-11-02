# Generate all the libraries in this folder

# Creates a new shared library - .dll windows / .so unix
# The library is also a "target" in cmake
add_library(
        streams SHARED
        stream/input_stream.cpp
        stream/output_stream.cpp
        stream/input_stdio_stream.cpp ../include/stream/input_stdio_stream.hpp stream/ouput_stdio_stream.cpp ../include/stream/ouput_stdio_stream.hpp)

target_link_libraries(streams PUBLIC ${Boost_LIBRARIES}) # links boost libraries

set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/database)
set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/output)
configure_file(resources_config.hpp.in ${CMAKE_SOURCE_DIR}/include/resources_config.hpp)
