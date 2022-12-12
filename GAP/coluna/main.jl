using JuMP, GLPK;
using BlockDecomposition, Coluna;

struct Instance

    M::Vector{Int}
    J::Vector{Int}
    c::Matrix{Float64}
    w::Matrix{Float64}
    Q::Vector{Float64}

   function Instance()

    M = 1:3;
    J = 1:15;
    c = [12.7 22.5 8.9 20.8 13.6 12.4 24.8 19.1 11.5 17.4 24.7 6.8 21.7 14.3 10.5; 19.1 24.8 24.4 23.6 16.1 20.6 15.0 9.5 7.9 11.3 22.6 8.0 21.5 14.7 23.2;  18.6 14.1 22.7 9.9 24.2 24.5 20.8 12.9 17.7 11.9 18.7 10.1 9.1 8.9 7.7];
    w = [61 70 57 82 51 74 98 64 86 80 69 79 60 76 78; 50 57 61 83 81 79 63 99 82 59 83 91 59 99 91;91 81 66 63 59 81 87 90 65 55 57 68 92 91 86];
    Q = [1020, 1460, 1530];

    new(M, J, c, w, Q)

   end

end

function make_model(instance::Instance)

    coluna = optimizer_with_attributes(
        Coluna.Optimizer,
        "params" => Coluna.Params(
            solver = Coluna.Algorithm.TreeSearchAlgorithm(
                conqueralg = Coluna.ColCutGenConquer(
                    stages = Coluna.ColumnGeneration[
                            Coluna.ColumnGeneration(
                            smoothing_stabilization = 0.0,
                            log_print_frequency = 0,
                        )
                    ],
                    primal_heuristics = Coluna.Algorithm.ParameterizedHeuristic[],
                    max_nb_cut_rounds = 0
                )
            )
        ),
        "default_optimizer" => GLPK.Optimizer # GLPK for the master & the subproblems
    );

    @axis(M_axis, instance.M);

    model = BlockModel(coluna);

    @variable(model, x[m in M_axis, j in instance.J], Bin);
    @constraint(model, cov[j in instance.J], sum(x[m, j] for m in M_axis) >= 1);
    @constraint(model, knp[m in M_axis], sum(instance.w[m, j] * x[m, j] for j in instance.J) <= instance.Q[m]);
    @objective(model, Min, sum(instance.c[m, j] * x[m, j] for m in M_axis, j in instance.J));

    @dantzig_wolfe_decomposition(model, decomposition, M_axis)

    master = getmaster(decomposition)
    subproblems = getsubproblems(decomposition)

    specify!.(subproblems, lower_multiplicity = 0, upper_multiplicity = 1)

    return model

end

instance = Instance()
model = make_model(instance)

# Solving once as warm up
optimize!(model)


# Actually solving

optimize!(model)

println( solve_time(model) )
println( objective_value(model) )

