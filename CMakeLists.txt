cmake_minimum_required(VERSION 3.23)
project(idol_benchmark)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)

FetchContent_Declare(
        idol
        GIT_REPOSITORY https://github.com/hlefebvr/idol.git
        GIT_TAG        main
)

FetchContent_MakeAvailable(idol)

add_executable(idol_benchmark main.cpp)
target_link_libraries(idol_benchmark PUBLIC idol)
