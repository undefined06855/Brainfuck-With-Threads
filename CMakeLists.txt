cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(BF2)

file(GLOB_RECURSE sources src/*.cpp)
add_executable(BF2 ${sources})
