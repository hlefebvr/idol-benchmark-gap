#include <iostream>

#include "solve_with_branch_and_price.h"
#include "solve_with_branch_and_bound.h"
#include "solve_with_glpk.h"

bool parse_bool(const std::string& t_string) {
    if (t_string == "true") {
        return true;
    }
    if (t_string == "false") {
        return false;
    }
    throw std::runtime_error("Expected true|false for boolean values.");
}

int main(int t_argc, const char** t_argv) {

    Logs::set_level<BranchAndBound>(Info);
    //Logs::set_level<DantzigWolfe>(Info);

    if (t_argc < 3) {
        throw std::runtime_error("Expected arguments: path_to_instance method [with_heuristics] [smoothing_factor] [farkas_pricing] [branching_on_master]");
    }

    constexpr double time_limit = 5 * 60;
    const std::string path_to_instance = t_argv[1];
    const std::string method = t_argv[2];

    if (method != "external" && method != "bab" && method != "bap") {
        throw std::runtime_error("Expected external|bab|bap for argument method.");
    }

    if (method == "external") {
        solve_with_external_solver(path_to_instance, time_limit);
        return 0;
    }

    if (t_argc < 4) {
        throw std::runtime_error("Expected argument 3: with_heuristics");
    }

    const bool with_heuristics = parse_bool(t_argv[3]);

    if (method == "bab") {
        solve_with_branch_and_bound(path_to_instance, time_limit, with_heuristics);
        return 0;
    }

    if (t_argc < 7) {
        throw std::runtime_error("Expected argument 4, 5, 6: smothing_factor farkas_pricing branching_on_master");
    }

    const double smoothing_factor = std::stod(t_argv[4]);
    const bool farkas_pricing = parse_bool(t_argv[5]);
    const bool branching_on_master = parse_bool(t_argv[6]);
    const int clean_up = 500;

    solve_with_branch_and_price(path_to_instance, time_limit, with_heuristics, smoothing_factor, farkas_pricing, clean_up, branching_on_master);

    return 0;
}
