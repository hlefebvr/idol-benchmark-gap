#include <iostream>

#include "solve_with_branch_and_price.h"
#include "solve_with_branch_and_bound.h"
#include "solve_with_glpk.h"


int main(int t_argc, const char** t_argv) {

    if (t_argc != 2) {
        throw std::runtime_error("Expected argument 1: path_to_instance");
    }

    constexpr double time_limit = 3600;
    const std::string path_to_instance = t_argv[1];

    Logs::set_level<BranchAndBound>(Info);
    Logs::set_level<DantzigWolfe>(Debug);

    solve_with_external_solver(path_to_instance, time_limit);

    for (const auto with_heuristics : { true, false }) {
        solve_with_branch_and_bound(path_to_instance, time_limit, with_heuristics);
    }

    for (const bool branchin_on_master : { true, false }) {
        for (const bool with_heuristics: {true, false}) {
            for (const bool farkas_pricing: {false, true}) {
                for (const double smoothing_factor: {0., .3, .8}) {
                    for (const int clean_up: {500}) {
                        solve_with_branch_and_price(path_to_instance,
                                                    time_limit,
                                                    with_heuristics,
                                                    smoothing_factor,
                                                    farkas_pricing,
                                                    clean_up,
                                                    branchin_on_master);
                    }

                }
            }
        }
    }

    return 0;
}
