using JuMP;
using BlockDecomposition, Coluna;
using DelimitedFiles;
using Base.Threads;

if (length(ARGS) != 2)
    throw(ArgumentError("Expected argument \"solver\" \"instance\""))
end

if (ARGS[1] == "mosek")
    using MosekTools;
    ExternalSolver = Mosek.Optimizer;
elseif (ARGS[1] == "glpk")
    using GLPK;
    # ExternalSolver = GLPK.Optimizer
    ExternalSolver = GLPK.Optimizer
elseif (ARGS[1] == "gurobi")
    using Gurobi;
    ExternalSolver = Gurobi.Optimizer
elseif (ARGS[1] == "highs")
    using HiGHS;
    ExternalSolver = HiGHS.Optimizer
else
    throw(ErrorException("Allowed values for parameter 1: mosek, glpk, highs, gurobi"));
end

file = ARGS[2]

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

function make_model(instance::Instance, time_limit::Int)

    coluna = optimizer_with_attributes(
        Coluna.Optimizer,
        "params" => Coluna.Params(
            solver = Coluna.Algorithm.TreeSearchAlgorithm(
                        conqueralg = Coluna.ColCutGenConquer(
                                    colgen =
                                            Coluna.ColumnGeneration(
                                                smoothing_stabilization = 0.0,
                                                log_print_frequency = 0,
                                                cleanup_threshold = 1500
                                            ),
                        primal_heuristics = Coluna.Algorithm.ParameterizedHeuristic[
                            Coluna.Algorithm.ParamRestrictedMasterHeuristic()
                        ],
                        max_nb_cut_rounds = 0
                ),
                timelimit = time_limit,
                explorestrategy = Coluna.TreeSearch.BestDualBoundStrategy(),
                opt_atol = 1e-5,
                opt_rtol = 1e-4
            )
        ),
        "default_optimizer" => ExternalSolver # Mosek for the master & the subproblems
    );

    @axis(M_axis, instance.M);

    model = BlockModel(coluna);

    @variable(model, x[m in M_axis, j in instance.J], Bin);
    @constraint(model, cov[j in instance.J], sum(x[m, j] for m in M_axis) == 1);
    @constraint(model, knp[m in M_axis], sum(instance.w[m, j] * x[m, j] for j in instance.J) <= instance.Q[m]);
    @objective(model, Min, sum(instance.c[m, j] * x[m, j] for m in M_axis, j in instance.J));

    @dantzig_wolfe_decomposition(model, decomposition, M_axis)

    master = getmaster(decomposition)
    subproblems = getsubproblems(decomposition)

    specify!.(subproblems, lower_multiplicity = 1, upper_multiplicity = 1)

    return model

end

println("Solving an easy problem to warm up...")

warmup_instance = "GAP/data/dummies/dummy"
instance = Instance(warmup_instance)
model = make_model(instance, 3600)
optimize!(model)

println("Switching to instance...")

function write_output(t_file::String, t_instance::Instance, t_status::String, t_objective::String, t_time::String)
    open("results_GAP_coluna.csv", "a+") do output
        write(output,
            t_file * ","
            * "Coluna.jl,"
            * "1," # with heuristic
            * "0," # smoothing
            * "0," # farkas
            * "500," # cleanup
            * "1," # branching on master
            * t_status * ","
            * t_objective * ","
            * t_time
            * "\n")
    end;
end

instance = Instance(file)

println("Solving " * file)

try

time_limit = 5 * 60

model = make_model(instance, time_limit)

optimize!(model)

println(solution_summary(model))

write_output(
    file,
    instance,
    string(termination_status(model)),
    termination_status(model) == OPTIMAL ? string(objective_value(model)) : "0",
    string(solve_time(model))
)

catch (error)
    println("FAILED.")

    write_output(
        file,
        instance,
        "ERROR",
        "0",
        "999999999999"
    )
end

