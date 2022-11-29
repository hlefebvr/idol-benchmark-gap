#include <iostream>
#include <fstream>
#include "modeling.h"
#include "algorithms.h"
#include "problems/GAP/GAP_Instance.h"

#define TIME_LIMIT 3600

using namespace Problems::GAP;

void write_to_file(
        const std::string& t_path_to_instance,
        const std::string& t_solver_name,
        unsigned int t_n_knapsacks,
        unsigned int t_n_items,
        double t_objective_value,
        double t_time
        ) {

    std::ofstream file("errors.txt", std::ios::out | std::ios::app);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open error destination file.");
    }

    file << t_path_to_instance << ','
         << t_solver_name << ','
         << t_n_knapsacks << ','
         << t_n_items << ','
         << t_objective_value << ','
         << t_time
         << '\n';

}

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
        restricted_master_problem.add_ctr(idol_Sum(j, Range(n_items), alphas[k]) == 1);
        restricted_master_problem.add_ctr(idol_Sum(j, Range(n_items), !x[k][j] * alphas[k]) == 1);
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
                        Solvers::GLPK_Simplex,
                        Solvers::GLPK_Simplex
                    >(restricted_master_problem, alphas, subproblems, std::move(branching_candidates));
    solver.solve();

    write_to_file(
            t_path_to_instance,
            "idol_bap",
            n_knapsacks,
            n_items,
            solver.objective_value(),
            solver.time().count()
        );
}

int main(int t_argc, const char** t_argv) {

    if (t_argc != 2) {
        throw std::runtime_error("Expected argument 1: path_to_instance");
    }

    const std::string path_to_instance = t_argv[1];

    solve_with_branch_and_price(path_to_instance);
    //solve_with_glpk(path_to_instance);

    return 0;
}
