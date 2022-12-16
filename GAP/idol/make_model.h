//
// Created by henri on 16/12/22.
//

#ifndef IDOL_BENCHMARK_MAKE_MODEL_H
#define IDOL_BENCHMARK_MAKE_MODEL_H

#include <problems/GAP/GAP_Instance.h>
#include <modeling.h>

std::tuple<Model, Vector<Var, 2>, UserAttr>
make_model(const Problems::GAP::Instance& t_instance, bool t_continuous_relaxation = false) {

    const unsigned int n_knapsacks = t_instance.n_agents();
    const unsigned int n_items = t_instance.n_jobs();

    Model model;
    auto complicating_constraint = model.add_user_attr<unsigned int>(0, "complicating_constraint");

    // Variables
    auto x = model.add_vars(Dim<2>(n_knapsacks, n_items), 0., 1., t_continuous_relaxation ? Continuous : Binary, 0., "x");

    // Objective function
    Expr objective = idol_Sum(
            i, Range(n_knapsacks),
            idol_Sum(j, Range(n_items), t_instance.cost(i, j) * x[i][j])
    );
    model.set(Attr::Obj::Expr, objective);

    // Knapsack constraints
    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        auto ctr = model.add_ctr( idol_Sum(j, Range(n_items), t_instance.resource_consumption(i, j) * x[i][j]) <= t_instance.capacity(i) );
        model.set<unsigned int>(complicating_constraint, ctr, i+1);
    }

    // Assignment constraints
    for (unsigned int j = 0 ; j < n_items ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_knapsacks), x[i][j]) == 1);
    }

    return std::make_tuple(std::move(model), x, complicating_constraint);

}

#endif //IDOL_BENCHMARK_MAKE_MODEL_H
