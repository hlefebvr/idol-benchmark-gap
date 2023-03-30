# Benchmarking idol

## Github Actions

**WARNING:** Currently, benchmarking with coluna.jl is not fair and plays in favor of idol. This is
because coluna.jl uses GLPK as external solver while idol uses Mosek. 
See [coluna.jl issue 793](https://github.com/atoptima/Coluna.jl/issues/793).

The benchmark is automatically done on GitHub Actions using the Mosek commercial solver. 
Note that the Mosek license is stored as a GitHub secret (with base64 encoding `cat mosek.lic | base64` to 
avoid corruption) and cannot publicly be accessed.
