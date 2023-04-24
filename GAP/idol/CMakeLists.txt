add_executable(benchmark_idol_gap main.cpp write_to_file.h)
target_link_libraries(benchmark_idol_gap PUBLIC idol)

target_compile_options(benchmark_idol_gap PRIVATE -g)
target_compile_options(benchmark_idol_gap PRIVATE -rdynamic)