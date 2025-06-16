cmake_minimum_required(VERSION 3.25)
project(MyProjectTests)

# Автоматическое скачивание doctest
include(FetchContent)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG origin/master)
FetchContent_MakeAvailable(doctest)

FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG origin/master)
FetchContent_MakeAvailable(stb)


# Include your project headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR}/_deps/stb-src/)


# Fetch STB and doctest


# Main executable
add_executable(stego_program main.cpp lev.cpp stb_impl.cpp)

# Test executable
add_executable(stego_tests test.cpp lev.cpp stb_impl.cpp)
target_link_libraries(stego_tests PRIVATE doctest::doctest)


enable_testing()
add_test(NAME stego_tests COMMAND stego_tests --force-colors -d)