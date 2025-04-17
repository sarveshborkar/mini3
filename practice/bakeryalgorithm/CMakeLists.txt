cmake_minimum_required(VERSION 2.8)

project (bakeryalgorithm)

SET(CMAKE_CXX_FLAGS "-O3 -std=c++11")

# app targets
add_executable(bakeryalgorithm bakeryalgorithm.cpp)
