cmake_minimum_required(VERSION 2.8)

project (dijkstrame)

SET(CMAKE_CXX_FLAGS "-O3 -std=c++11")

# app targets
add_executable(dijkstrame dijkstrame.cpp)
