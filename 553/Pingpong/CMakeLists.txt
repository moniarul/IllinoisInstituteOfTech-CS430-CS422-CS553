cmake_minimum_required(VERSION 3.9)
project(Pingpong)
set(CMAKE_CXX_FLAGS "-g -O3 -Wall -lpthread")
set(CMAKE_CXX_STANDARD 11)

add_executable(Pingpong main.cpp)