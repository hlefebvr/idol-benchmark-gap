using JuMP, GLPK;
using BlockDecomposition, Coluna;
using DelimitedFiles;

struct Instance

    M::Vector{Int}
    J::Vector{Int}
    c::Matrix{Float64}
    w::Matrix{Float64}
    Q::Vector{Float64}

   function Instance(t_filename::String)

   data = readdlm(t_filename)

    n_agents = data[1,1]
    n_jobs = data[1,2]

    M = 1:n_agents;
    J = 1:n_jobs;
    c = data[2:(n_agents+1),1:n_jobs];
    w = data[(2+n_agents):(1+n_agents+n_agents), 1:n_jobs];
    Q = data[(2+n_agents+n_agents), 1:n_agents];

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

instance = Instance("/home/henri/CLionProjects/idol_benchmark/GAP/data/generated/instance_n3_50__3.txt")
model = make_model(instance)

# Solving once as warm up
optimize!(model)


# Actually solving

optimize!(model)

solution_summary(model, verbose=true)

println( solve_time(model) )
println( objective_value(model) )

