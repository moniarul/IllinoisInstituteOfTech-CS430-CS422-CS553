cmake_minimum_required(VERSION 3.9)
project(network)

set(CMAKE_CXX_FLAGS "-g -O3 -Wall -lpthread")
set(CMAKE_CXX_STANDARD 11)

add_executable(network main.cpp)
# g++ -std=gnu++14  -O3  -pthread -Wall -o ./cs553-pa1/network/MyNETBench-TCP ./cs553-pa1/network/MyNETBench-TCP.cpp
# srun -n 1 -p interactive --pty /bin/bash

# ./cs553-pa1/network/MyNETBench-TCP server 4 268435456