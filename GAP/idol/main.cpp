#include <iostream>

#include <idol/modeling.h>
#include <idol/solvers.h>
#include <idol/problems/generalized-assignment-problem/GAP_Instance.h>
#include <idol/optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h>
#include <idol/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h>
#include "write_to_file.h"

#define OPTIMIZER HiGHS

///////////////////////////
#include <cstdio>
#include <execinfo.h>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <idol/optimizers/branch-and-bound/BranchAndBound.h>
#include <idol/optimizers/callbacks/heuristics/RENS.h>
#include <idol/optimizers/callbacks/heuristics/IntegerMaster.h>
#include <idol/optimizers/callbacks/cutting-planes/KnapsackCover.h>

using namespace idol;

void handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 30);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}
///////////////////////////

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

    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);

    if (t_argc < 3) {
        throw std::runtime_error("Expected arguments: path_to_instance method [with_heuristics] [smoothing_factor] [farkas_pricing] [branching_on_master]");
    }

    // Execution parameters
    constexpr double time_limit = 5 * 60;
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
    auto x = model.add_vars(Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");
    auto art = model.add_var(0,0,Continuous, "art");

    // Objective function
    Expr objective = idol_Sum(
            i, Range(n_agents),
            idol_Sum(j, Range(n_jobs), t_instance.cost(i, j) * x[i][j])
    );
    model.set_obj_expr(std::move(objective));

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

        model.use(OPTIMIZER().with_time_limit(time_limit));

    } else if (method == "bab") {

        if (t_argc < 4) {
            throw std::runtime_error("Expected argument 3: with_heuristics");
        }

        with_heuristics = parse_bool(t_argv[3]);

        model.use(
                BranchAndBound()
                        .with_node_optimizer(OPTIMIZER::ContinuousRelaxation())
                        .with_branching_rule(MostInfeasible())
                        .with_node_selection_rule(BestBound())
                        .add_callback(Cuts::KnapsackCover())
                        .with_time_limit(time_limit)
                        .with_subtree_depth(0)
                        .with_logs(true)
                        .conditional(with_heuristics, [](auto& x) {
                            x.add_callback(
                                    Heuristics::RENS()
                                            .with_optimizer(
                                                    BranchAndBound()
                                                            .with_node_optimizer(OPTIMIZER::ContinuousRelaxation())
                                                            .with_branching_rule(MostInfeasible())
                                                            .with_node_selection_rule(BestBound())
                                                            .add_callback(Cuts::KnapsackCover())
                                                            .with_time_limit(time_limit)
                                            )
                            );
                        })
        );

    } else if (method == "bap") {


        if (t_argc < 7) {
            throw std::runtime_error("Expected argument 4, 5, 6: smothing_factor farkas_pricing branching_on_master");
        }

        with_heuristics = parse_bool(t_argv[3]);
        smoothing_factor = std::stod(t_argv[4]);
        with_farkas_pricing = parse_bool(t_argv[5]);
        branching_on_master = parse_bool(t_argv[6]);
        clean_up = 1500;

        std::unique_ptr<DantzigWolfe::InfeasibilityStrategyFactory> infeasibility_strategy(with_farkas_pricing ?
               (DantzigWolfe::InfeasibilityStrategyFactory*) new DantzigWolfe::FarkasPricing() :
               (DantzigWolfe::InfeasibilityStrategyFactory*) new DantzigWolfe::ArtificialCosts()
        );

        model.use(
                BranchAndBound()
                        .with_node_optimizer(
                                DantzigWolfeDecomposition(decomposition)
                                        .with_master_optimizer(OPTIMIZER::ContinuousRelaxation().with_logs(false))
                                        .with_default_sub_problem_spec(
                                                DantzigWolfe::SubProblem()
                                                        .add_optimizer(OPTIMIZER().with_logs(false))
                                                        .with_column_pool_clean_up(clean_up, .75)
                                                        .with_max_column_per_pricing(10)
                                        )
                                        .with_dual_price_smoothing_stabilization(DantzigWolfe::Neame(smoothing_factor))
                                        .with_hard_branching(!branching_on_master)
                                        .with_infeasibility_strategy(*infeasibility_strategy)
                                        .with_max_parallel_sub_problems(1)
                                        .with_logs(false)
                                        // .with_log_frequency(1)
                        )
                        .with_branching_rule(MostInfeasible())
                        .with_node_selection_rule(BestBound())
                        .with_time_limit(time_limit)
                        .conditional(with_heuristics, [](auto& x){
                            x.add_callback(
                                    Heuristics::IntegerMaster()
                                            .with_optimizer(
                                                OPTIMIZER().with_logs(false)
                                                    .with_presolve(false)
                                                    .with_time_limit(10)
                                            )
                            );
                        })
                        .with_subtree_depth(0)
                        .with_logs(true)
        );

    } else {

        throw std::runtime_error("Expected external|bab|bap for argument method.");

    }

    model.optimize();

    write_results_to_file(
            path_to_instance,
            method,
            with_heuristics,
            smoothing_factor,
            with_farkas_pricing,
            clean_up,
            branching_on_master,
            model.get_status(),
            model.get_reason(),
            model.get_best_obj(),
            model.optimizer().time().count()
    );

    if (method == "bap") {

        const auto &branch_and_bound = model.optimizer().as<Optimizers::BranchAndBound<DefaultNodeInfo>>();
        const auto &dantzig_wolfe = branch_and_bound.relaxation().optimizer().as<Optimizers::DantzigWolfeDecomposition>();

        const double total_time = model.optimizer().time().count();
        const double bab_time = branch_and_bound.time().count();
        const double cg_time = branch_and_bound.relaxation().optimizer().time().cumulative_count();
        const double master_time = dantzig_wolfe.formulation().master().optimizer().time().cumulative_count();
        double sp_time = 0.;

        for (auto &sp: dantzig_wolfe.formulation().sub_problems()) {
            sp_time += sp.optimizer().time().cumulative_count();
        }

        std::cout << "Total: " << total_time << std::endl;
        std::cout << "BaB: " << bab_time << std::endl;
        std::cout << "CG: " << cg_time << std::endl;
        std::cout << "Master: " << master_time << std::endl;
        std::cout << "SP: " << sp_time << std::endl;
        std::cout << "--" << std::endl;
        std::cout << "% of time spent actually solving opt. problems: " << (master_time + sp_time) / total_time * 100  << " s" << std::endl;

        write_bap_annex_results_to_file(
                path_to_instance,
                method,
                with_heuristics,
                smoothing_factor,
                with_farkas_pricing,
                clean_up,
                branching_on_master,
                total_time,
                bab_time,
                cg_time,
                master_time,
                sp_time
        );

    }

    return 0;
}
