//
// Created by henri on 29/11/22.
//

#ifndef IDOL_BENCHMARK_SOLVE_WITH_BRANCH_AND_PRICE_H
#define IDOL_BENCHMARK_SOLVE_WITH_BRANCH_AND_PRICE_H

#include "write_to_file.h"
#include "modeling.h"
#include "algorithms.h"
#include "problems/GAP/GAP_Instance.h"
#include "algorithms/callbacks/Callbacks_RoundingHeuristic.h"

using namespace Problems::GAP;

void solve_with_branch_and_price(const std::string& t_path_to_instance) {

    const auto instance = read_instance(t_path_to_instance);

    const unsigned int n_knapsacks = instance.n_knapsacks();
    const unsigned int n_items = instance.n_items();

    // Create sub problems
    std::vector<Model> subproblems(n_knapsacks);
    std::vector<Vector<Var>> x(n_knapsacks);

    for (unsigned int k = 0 ; k < n_knapsacks ; ++k) {
        x[k] = subproblems[k].add_vars(Dim<1>(n_items), 0., 1., Continuous, 0.);
        subproblems[k].add_ctr(idol_Sum(j, Range(n_items), instance.w(k, j) * x[k][j]) <= instance.t(k));
    }

    // Create restricted master problem
    Model restricted_master_problem;
    auto alphas = restricted_master_problem.add_vars(Dim<1>(n_knapsacks), 0., 1., Continuous, 0.);

    for (unsigned int k = 0 ; k < n_knapsacks ; ++k) {
        Constant objective = idol_Sum(j, Range(n_items), instance.p(k, j) * !x[k][j]).constant();
        restricted_master_problem.set(Attr::Var::Obj, alphas[k], std::move(objective));
        restricted_master_problem.add_ctr(alphas[k] == 1);
    }
    for (unsigned int j = 0 ; j < n_items ; ++j) {
        restricted_master_problem.add_ctr(idol_Sum(k, Range(n_knapsacks), !x[k][j] * alphas[k]) == 1);
    }

    // Create branching candidates
    std::vector<Var> branching_candidates;
    branching_candidates.reserve(n_knapsacks * n_items);

    for (unsigned int k = 0 ; k < n_knapsacks ; ++k) {
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            branching_candidates.emplace_back(x[k][j]);
        }
    }

    // Create solver
    auto solver = branch_and_price<
            Solvers::GLPK,
            Solvers::GLPK
    >(restricted_master_problem, alphas, subproblems, branching_candidates);
    solver.add_callback<Callbacks::RoundingHeuristic>(branching_candidates);
    solver.set(Param::Algorithm::TimeLimit, TIME_LIMIT);
    solver.solve();

    write_to_file(
            t_path_to_instance,
            "idol_bap",
            n_knapsacks,
            n_items,
            solver.primal_solution().objective_value(),
            solver.time().count()
    );
}

#endif //IDOL_BENCHMARK_SOLVE_WITH_BRANCH_AND_PRICE_H
