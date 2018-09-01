cmake_minimum_required(VERSION 3.9)
project(cpu2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -Wall -O3 -lpthread -mavx2 -mfma")

add_executable(cpu2 main.cpp)
#g++ -std=c++0x  -o MyCPUBench  MyCPUBench.cpp -lpthread