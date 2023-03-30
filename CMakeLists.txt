cmake_minimum_required(VERSION 3.23)
project(idol_benchmark)

set(CMAKE_CXX_STANDARD 17)

set(USE_MOSEK YES)
set(USE_EIGEN YES)
set(USE_GLPK YES)

include(FetchContent)

FetchContent_Declare(
        idol
        GIT_REPOSITORY https://github.com/hlefebvr/idol.git
        GIT_TAG        origin/main
)

FetchContent_MakeAvailable(idol)

add_subdirectory(GAP)