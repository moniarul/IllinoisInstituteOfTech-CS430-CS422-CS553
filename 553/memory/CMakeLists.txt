cmake_minimum_required(VERSION 3.9)
project(memory)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -Wall -O3 -lpthread -mavx")

add_executable(memory main.cpp)
#g++ -std=c++0x  -o MyCPUBench  MyCPUBench.cpp -lpthread -mavx