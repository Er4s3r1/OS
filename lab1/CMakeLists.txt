cmake_minimum_required(VERSION 3.0.0)
project(lab1 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

add_executable(parent parent.cpp)
add_executable(child child.cpp)
