using JuMP;
using BlockDecomposition, Coluna;
using DelimitedFiles;

if (length(ARGS) != 2)
    throw(ArgumentError("Expected argument \"solver\" \"instance_folder\""))
end

if (ARGS[1] == "glpk")
    using GLPK;
elseif (ARGS[1] == "gurobi")
    using Gurobi;
else
    throw(ErrorException("Allowed values for parameter 1: glpk, gurobi"));
end

instance_folder = ARGS[2]

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
                            stages = Coluna.ColumnGeneration[
                                        Coluna.ColumnGeneration(
                                            smoothing_stabilization = 0.0,
                                            log_print_frequency = 0,
                                            cleanup_threshold = 1500
                                        )
                    ],
                    max_nb_cut_rounds = 0
                ),
                timelimit = time_limit
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

all_instances = readdir(instance_folder)


println("Solving an easy problem to warm up...")

warmup_instance = instance_folder * "/" * all_instances[1]
instance = Instance(warmup_instance)
model = make_model(instance, 3600)
optimize!(model)

println("Switching to test bed...")

function write_output(t_file::String, t_instance::Instance, t_status::String, t_objective::String, t_time::String)
    open("results_GAP_coluna.csv", "a+") do output
        write(output,
            instance_folder * "/" * t_file * ","
            * "coluna,"
            * "1," # with heuristic
            * "0," # smoothing
            * "0," # farkas
            * "500," # cleanup
            * "1," # branching on master
            * t_status * ","
            * ","
            * string(length(t_instance.M)) * ","
            * string(length(t_instance.J)) * ","
            * t_objective * ","
            * t_time
            * "\n")
    end;
end

foreach(all_instances) do file

    instance = Instance(instance_folder * "/" * file)
    model = make_model(instance, 5 * 60)

    try
        optimize!(model)

        write_output(
            file,
            instance,
            string(termination_status(model)),
            string(objective_value(model)),
            string(solve_time(model))
        )

    catch (error)
        write_output(
            file,
            "ERROR",
            "0",
            "999999999999"
        )
    end

end
