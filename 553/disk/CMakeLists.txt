cmake_minimum_required(VERSION 3.9)
project(disk)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -Wall -lpthread")
add_executable(disk main.cpp)

#g++ -std=c++0x  -O3 -lpthread -o MyRAMBench MyRAMBench.cpp