add_executable(benchmark_idol_gap main.cpp solve_with_branch_and_price.h write_to_file.h solve_with_glpk.h solve_with_branch_and_bound.h make_model.h)
target_link_libraries(benchmark_idol_gap PUBLIC idol)

if (${USE_GUROBI})
    add_compile_definitions(benchmark_idol_gap SOLVER=Solvers::Gurobi)
else()
    add_compile_definitions(benchmark_idol_gap SOLVER=Solvers::GLPK)
endif()
