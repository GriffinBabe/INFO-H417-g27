# Generate all the libraries in this folder

# Creates a new shared library - .dll windows / .so unix
# The library is also a "target" in cmake
add_library(
        streams SHARED
        stream/input_stream.cpp
        stream/output_stream.cpp
        stream/simple_input_stream.cpp)

target_link_libraries(streams PUBLIC ${Boost_LIBRARIES}) # links boost libraries
