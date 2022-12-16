//
// Created by henri on 29/11/22.
//

#ifndef IDOL_BENCHMARK_SOLVE_WITH_GLPK_H
#define IDOL_BENCHMARK_SOLVE_WITH_GLPK_H

#include "write_to_file.h"
#include "make_model.h"

using namespace Problems::GAP;

void solve_with_external_solver(const std::string& t_path_to_instance, double t_time_limit) {

    std::cout << "solve_with_external_solver with time_limit = " << t_time_limit << std::endl;

    const auto instance = Problems::GAP::read_instance(t_path_to_instance);

    auto [model, x, complicating_constraints] = make_model(instance);

    SOLVER solver(model);
    solver.set(Param::Algorithm::TimeLimit, t_time_limit);
    solver.solve();

    write_to_file(
            t_path_to_instance,
            instance,
            "external_solver",
            false,
            0.,
            false,
            0,
            false,
            solver.status(),
            solver.reason(),
            solver.get(Attr::Solution::ObjVal),
            solver.time().count()
    );
}

#endif //IDOL_BENCHMARK_SOLVE_WITH_GLPK_H
