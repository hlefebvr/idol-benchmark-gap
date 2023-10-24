//
// Created by henri on 29/11/22.
//

#ifndef IDOL_BENCHMARK_WRITE_TO_FILE_H
#define IDOL_BENCHMARK_WRITE_TO_FILE_H

#include <idol/problems/generalized-assignment-problem/GAP_Instance.h>
#include <fstream>
#include <idol/modeling/solutions/types.h>

void write_results_to_file(
        const std::string& t_path_to_instance,
        const std::string& t_optimizer_name,
        bool t_with_heuristics,
        double t_smoothing_factor,
        bool t_farkas_pricing,
        unsigned int t_clean_up,
        bool t_branching_on_master,
        idol::SolutionStatus t_status,
        idol::SolutionReason t_reason,
        double t_objective_value,
        double t_time
) {

    std::ofstream file("results_GAP_idol.csv", std::ios::out | std::ios::app);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open error destination file.");
    }

    file << t_path_to_instance << ','
         << (t_optimizer_name == "external" ? t_optimizer_name : "idol_" + t_optimizer_name) << ','
         << t_with_heuristics << ','
         << t_smoothing_factor << ','
         << t_farkas_pricing << ','
         << t_clean_up << ','
         << t_branching_on_master << ','
         << t_status << ','
         << t_reason << ','
         << t_objective_value << ','
         << t_time
         << '\n';

    file.close();
}

void write_bap_annex_results_to_file(
        const std::string& t_path_to_instance,
        const std::string& t_optimizer_name,
        bool t_with_heuristics,
        double t_smoothing_factor,
        bool t_farkas_pricing,
        unsigned int t_clean_up,
        bool t_branching_on_master,
        double t_total_time,
        double t_bab_time,
        double t_cg_time,
        double t_master_time,
        double t_sp_time
        ) {

    std::ofstream file("bap_annex_GAP_idol.csv", std::ios::out | std::ios::app);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open error destination file.");
    }

    file << t_path_to_instance << ','
         << "idol_" + t_optimizer_name << ','
         << t_with_heuristics << ','
         << t_smoothing_factor << ','
         << t_farkas_pricing << ','
         << t_clean_up << ','
         << t_branching_on_master << ','
         << t_total_time << ','
         << t_bab_time << ','
         << t_cg_time << ','
         << t_master_time << ','
         << t_sp_time
         << '\n';

    file.close();
}

#endif //IDOL_BENCHMARK_WRITE_TO_FILE_H
