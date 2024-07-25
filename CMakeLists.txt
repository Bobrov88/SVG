cmake_minimum_required(VERSION 3.20)

project(svg)

add_executable(svg svg.cpp main.cpp)

set_target_properties(svg
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)