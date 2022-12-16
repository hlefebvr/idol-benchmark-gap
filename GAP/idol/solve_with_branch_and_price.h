//
// Created by henri on 29/11/22.
//

#ifndef IDOL_BENCHMARK_SOLVE_WITH_BRANCH_AND_PRICE_H
#define IDOL_BENCHMARK_SOLVE_WITH_BRANCH_AND_PRICE_H

#include "write_to_file.h"
#include "make_model.h"
#include "algorithms/callbacks/Callbacks_RoundingHeuristic.h"
#include "algorithms/dantzig-wolfe/DantzigWolfe.h"
#include "algorithms/dantzig-wolfe/BranchingManagers_OnMaster.h"
#include "algorithms/dantzig-wolfe/BranchingManagers_OnPricing.h"
#include "algorithms.h"

using namespace Problems::GAP;

void solve_with_branch_and_price(const std::string& t_path_to_instance,
                                 double t_time_limit,
                                 bool t_with_heuristics,
                                 double t_smoothing_factor,
                                 bool t_farkas_pricing,
                                 int t_clean_up,
                                 bool t_branching_on_master) {

    std::cout << "solve_with_branch_and_price with time_limit = " << t_time_limit
              << ", with_heuristics = " << t_with_heuristics
              << ", smoothing_factor = " << t_smoothing_factor
              << ", farkas_pricing = " << t_farkas_pricing
              << ", clean_up = " << t_clean_up
              << ", branching_on_master = " << t_branching_on_master
              << std::endl;

    const auto instance = Problems::GAP::read_instance(t_path_to_instance);

    auto [model, x, complicating_constraints] = make_model(instance);

    const auto branching_candidates = flatten<Var, 2>(x);

    BranchAndBound solver;

    auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
    node_strategy.set_active_node_manager_strategy<ActiveNodesManagers::Basic>();
    node_strategy.set_branching_strategy<BranchingStrategies::MostInfeasible>( branching_candidates );
    node_strategy.set_node_updator_strategy<NodeUpdators::ByBoundVar>();

    auto& dantzig_wolfe = solver.set_solution_strategy<DantzigWolfe>(model, complicating_constraints);

    dantzig_wolfe.set(Param::DantzigWolfe::CleanUpThreshold, t_clean_up);
    dantzig_wolfe.set(Param::DantzigWolfe::SmoothingFactor, t_smoothing_factor);
    dantzig_wolfe.set(Param::DantzigWolfe::FarkasPricing, t_farkas_pricing);
    dantzig_wolfe.set(Param::DantzigWolfe::LogFrequency, 1);

    auto& master = dantzig_wolfe.set_master_solution_strategy<Solvers::GLPK>();
    master.set(Param::Algorithm::InfeasibleOrUnboundedInfo, true);

    for (unsigned int i = 1, n = dantzig_wolfe.reformulation().subproblems().size() ; i <= n ; ++i) {
        dantzig_wolfe.subproblem(i).set_exact_solution_strategy<Solvers::GLPK>();
        if (t_branching_on_master) {
            dantzig_wolfe.subproblem(i).set_branching_manager<BranchingManagers::OnMaster>();
        } else {
            dantzig_wolfe.subproblem(i).set_branching_manager<BranchingManagers::OnPricing>();
        }
    }

    if (t_with_heuristics) {
        solver.add_callback<Callbacks::RoundingHeuristic>(branching_candidates);
    }
    solver.set(Param::Algorithm::TimeLimit, t_time_limit);

    solver.solve();

    write_to_file(
            t_path_to_instance,
            instance,
            "idol_bap",
            t_with_heuristics,
            t_smoothing_factor,
            t_farkas_pricing,
            t_clean_up,
            t_branching_on_master,
            solver.status(),
            solver.reason(),
            solver.get(Attr::Solution::ObjVal),
            solver.time().count()
    );
}

#endif //IDOL_BENCHMARK_SOLVE_WITH_BRANCH_AND_PRICE_H
