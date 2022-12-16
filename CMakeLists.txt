cmake_minimum_required(VERSION 3.23)
project(idol_benchmark)

set(CMAKE_CXX_STANDARD 17)

if (NOT DEFINED USE_GUROBI)
    set(USE_GLPK YES)
    message("Using GLPK")
endif()

include(FetchContent)

FetchContent_Declare(
        idol
        GIT_REPOSITORY https://github.com/hlefebvr/idol.git
        GIT_TAG        origin/dev
)

FetchContent_MakeAvailable(idol)

add_subdirectory(GAP)