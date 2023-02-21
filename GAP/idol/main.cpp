#include <iostream>

#include "modeling.h"
#include "solvers.h"
#include "problems/generalized-assignment-problem/GAP_Instance.h"
#include "write_to_file.h"

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

    Logs::set_level<BranchAndBound>(Debug);
    Logs::set_level<ColumnGeneration>(Info);

    if (t_argc < 3) {
        throw std::runtime_error("Expected arguments: path_to_instance method [with_heuristics] [smoothing_factor] [farkas_pricing] [branching_on_master]");
    }

    // Execution parameters
    constexpr double time_limit = 10 * 60;
    const std::string path_to_instance = t_argv[1];
    const std::string method = t_argv[2];

    std::cout << "solve with " << method << ", time_limit = " << time_limit << std::endl;

    // Default parameters
    bool with_heuristics = false;
    double smoothing_factor = 0.;
    bool with_farkas_pricing = false;
    int clean_up = 0;
    bool branching_on_master = false;

    // Read instance
    const auto t_instance = Problems::GAP::read_instance(path_to_instance);

    // Make model
    const unsigned int n_agents = t_instance.n_agents();
    const unsigned int n_jobs = t_instance.n_jobs();

    Env t_env;
    Model model(t_env);
    Annotation<Ctr, unsigned int> decomposition(t_env, "decomposition", MasterId);

    // Variables
    auto x = Var::array(t_env, Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");
    model.add_array<Var, 2>(x);

    // Objective function
    Expr objective = idol_Sum(
            i, Range(n_agents),
            idol_Sum(j, Range(n_jobs), t_instance.cost(i, j) * x[i][j])
    );
    model.set(Attr::Obj::Expr, std::move(objective));

    // Capacity constraints
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity(t_env, idol_Sum(j, Range(n_jobs), t_instance.resource_consumption(i, j) * x[i][j]) <= t_instance.capacity(i), "knapsack_" + std::to_string(i) );
        capacity.set(decomposition, i);
        model.add(capacity);
    }

    // Assignment constraints
    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        Ctr assignment(t_env, idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
        model.add(assignment);
    }


    // Set optimizer
    if (method == "external") {

        Idol::set_optimizer<GLPK>(model);

    } else if (method == "bab") {

        if (t_argc < 4) {
            throw std::runtime_error("Expected argument 3: with_heuristics");
        }

        with_heuristics = parse_bool(t_argv[3]);

        Idol::set_optimizer<BranchAndBoundMIP<GLPK>>(model);

    } else if (method == "bap") {


        if (t_argc < 7) {
            throw std::runtime_error("Expected argument 4, 5, 6: smothing_factor farkas_pricing branching_on_master");
        }

        with_heuristics = parse_bool(t_argv[3]);
        smoothing_factor = std::stod(t_argv[4]);
        with_farkas_pricing = parse_bool(t_argv[5]);
        branching_on_master = parse_bool(t_argv[6]);
        clean_up = 1500;

        Idol::set_optimizer<BranchAndPriceMIP<GLPK>>(model, decomposition);

        model.set(Param::ColumnGeneration::SmoothingFactor, smoothing_factor);
        model.set(Param::ColumnGeneration::FarkasPricing, with_farkas_pricing);
        model.set(Param::ColumnGeneration::BranchingOnMaster, branching_on_master);
        model.set(Param::ColumnGeneration::CleanUpThreshold, clean_up);
        model.set(Param::BranchAndPrice::IntegerMasterHeuristic, with_heuristics);

    } else {

        throw std::runtime_error("Expected external|bab|bap for argument method.");

    }

    // Set time limit
    model.set(Param::Algorithm::TimeLimit, time_limit);

    model.optimize();

    write_to_file(
            path_to_instance,
            t_instance,
            method,
            with_heuristics,
            smoothing_factor,
            with_farkas_pricing,
            clean_up,
            branching_on_master,
            (SolutionStatus) model.get(Attr::Solution::Status),
            (SolutionReason) model.get(Attr::Solution::Reason),
            model.get(Attr::Solution::ObjVal),
            model.time().count()
    );

    return 0;
}
