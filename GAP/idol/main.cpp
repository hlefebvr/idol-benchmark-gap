#include <iostream>
#include "solve_with_branch_and_price.h"
#include "solve_with_branch_and_bound.h"
#include "solve_with_glpk.h"

int main(int t_argc, const char** t_argv) {

    if (t_argc != 2) {
        throw std::runtime_error("Expected argument 1: path_to_instance");
    }

    const std::string path_to_instance = t_argv[1];

    Log::set_level(Mute);

    solve_with_glpk(path_to_instance);
    solve_with_branch_and_bound(path_to_instance);
    //solve_with_branch_and_price(path_to_instance);

    return 0;
}
