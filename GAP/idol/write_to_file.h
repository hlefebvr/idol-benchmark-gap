//
// Created by henri on 29/11/22.
//

#ifndef IDOL_BENCHMARK_WRITE_TO_FILE_H
#define IDOL_BENCHMARK_WRITE_TO_FILE_H

#include <fstream>

#define TIME_LIMIT 3600

void write_to_file(
        const std::string& t_path_to_instance,
        const std::string& t_solver_name,
        unsigned int t_n_knapsacks,
        unsigned int t_n_items,
        double t_objective_value,
        double t_time
) {

    std::ofstream file("results_GAP_idol.csv", std::ios::out | std::ios::app);

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

#endif //IDOL_BENCHMARK_WRITE_TO_FILE_H
