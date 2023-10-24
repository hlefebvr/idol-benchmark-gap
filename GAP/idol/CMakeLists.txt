add_executable(benchmark_idol_gap main.cpp write_to_file.h)

target_compile_options(benchmark_idol_gap PRIVATE -g -rdynamic)

set_target_properties(benchmark_idol_gap PROPERTIES
        C_COMPILER gcc
        CXX_COMPILER g++
)

target_link_libraries(benchmark_idol_gap PUBLIC idol)