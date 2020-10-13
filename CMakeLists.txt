cmake_minimum_required(VERSION 3.16)
project(info-h417)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED  True)

# For windows MSVC runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Looks for google test in your system
find_package(GTest)
if (${GTEST_FOUND})
else()
    message(FATAL_ERROR Couldn't find GTest)
endif()

# Looks for Boost in your system
find_package(Boost)
if (${Boost_FOUND})
else()
    message(FATAL_ERROR Couldn't find Boost)
endif()

# The directory of all includes. Will be accessible from everywhere in the cmake project and subprojects.
include_directories(include)

add_subdirectory(sources)
add_subdirectory(test)