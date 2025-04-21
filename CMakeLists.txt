cmake_minimum_required(VERSION 3.14)
project(MyProject)

add_executable(election_algorithm
  src/main.cpp
  src/Network.cpp
  src/Node.cpp
)
target_include_directories(election_algorithm PRIVATE include)