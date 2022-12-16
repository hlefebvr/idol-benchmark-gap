//
// Created by henri on 13/12/22.
//
#include <iostream>
#include <fstream>
#include "problems/GAP/GAP_Instance.h"
#include "errors/Exception.h"

int main(int t_argc, const char** t_argv) {

    if (t_argc != 2) {
        throw Exception("Expected argument DESTINATION_FOLDER");
    }

    const std::string output_dir = t_argv[1];

    const std::vector<std::pair<unsigned int, unsigned int>> sizes = {
            { 2, 30 },
            { 3, 30 },
    };

    for (const auto& [n_agents, n_jobs] : sizes) {
        for (unsigned int k = 0 ; k < 20 ; ++k) {
            auto instance = Problems::GAP::generate_instance_Chu_and_Beasley_1997_C(n_agents, n_jobs);

            const std::string filename = output_dir + "/instance_n" + std::to_string(n_agents) + "_" + std::to_string(n_jobs) + "__" + std::to_string(k) + ".txt";

            std::ofstream file(filename);

            if (!file.is_open()) {
                throw Exception("Could not open destination file: " + filename);
            }

            file << instance;

            file.close();
        }
    }

    return 0;
}
