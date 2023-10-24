cmake_minimum_required(VERSION 3.20)
project(idol_benchmark_gap)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_C_COMPILER gcc)
#set(CMAKE_CPP_COMPILER g++)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

find_package(idol REQUIRED)

add_subdirectory(GAP)
