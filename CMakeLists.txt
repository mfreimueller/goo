cmake_minimum_required(VERSION 3.31)
project(goo)

set(CMAKE_CXX_STANDARD 20)

find_package(CLI11 CONFIG REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)

target_link_libraries(goo PRIVATE CLI11::CLI11)