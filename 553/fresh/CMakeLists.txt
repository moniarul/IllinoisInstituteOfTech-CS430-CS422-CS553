cmake_minimum_required(VERSION 3.9)
project(fresh)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -Wall -lpthread")

add_executable(fresh frstMissing.cpp)

#g++ -std=c++0x  -o MyCPUBench  MyCPUBench.cpp -lpthread
