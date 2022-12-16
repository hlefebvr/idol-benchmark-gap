//
// Created by henri on 29/11/22.
//

#ifndef IDOL_BENCHMARK_SOLVE_WITH_BRANCH_AND_BOUND_H
#define IDOL_BENCHMARK_SOLVE_WITH_BRANCH_AND_BOUND_H

#include "write_to_file.h"
#include "make_model.h"
#include "algorithms/callbacks/Callbacks_RoundingHeuristic.h"
#include "algorithms.h"

using namespace Problems::GAP;

void solve_with_branch_and_bound(const std::string& t_path_to_instance,
                                 double t_time_limit,
                                 bool t_with_heuristics) {

    std::cout << "solve_with_branch_and_bound with time_limit = " << t_time_limit
              << ", with_heuristics = " << t_with_heuristics << std::endl;

    const auto instance = Problems::GAP::read_instance(t_path_to_instance);

    auto [model, x, complicating_constraints] = make_model(instance, true);

    const auto branching_candidates = flatten<Var, 2>(x);

    auto solver = branch_and_bound<SOLVER>(model, branching_candidates );
    solver.add_callback<Callbacks::RoundingHeuristic>( branching_candidates );
    if (t_with_heuristics) {
        solver.set(Param::Algorithm::TimeLimit, t_time_limit);
    }
    solver.solve();

    write_to_file(
            t_path_to_instance,
            instance,
            "idol_bab",
            t_with_heuristics,
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

#endif //IDOL_BENCHMARK_SOLVE_WITH_BRANCH_AND_BOUND_H
