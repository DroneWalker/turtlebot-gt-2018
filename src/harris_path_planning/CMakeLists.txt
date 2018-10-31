cmake_minimum_required(VERSION 3.5)
project(harris_path_planning)

set(CMAKE_CXX_STANDARD 14)

add_library(pathplanning implementation.cpp)

add_executable(harris_path_planning main.cpp)
add_dependencies(harris_path_planning pathplanning)