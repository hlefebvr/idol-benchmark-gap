# Benchmarking branch-and-price implementations

![Maintained](https://img.shields.io/maintenance/yes/2023)
![GitHub issues](https://img.shields.io/github/issues-raw/hlefebvr/idol_benchmark)
![GitHub Workflow Status (branch)](https://img.shields.io/github/actions/workflow/status/hlefebvr/idol_benchmark/cmake.yml?branch=main)

This code contains the code for benchmarking different branch-and-price implementations.
Currently, the comparison is done between the [idol C++ library](https://github.com/hlefebvr/idol) and
the [coluna.jl](https://github.com/atoptima/Coluna.jl) package. The external solver used to solve every
involved sub-problem is [the open-source GLPK solver](https://www.gnu.org/software/glpk/).

## Results

- [On Generalized Assignment Problem (GAP) instances](https://hlefebvr.github.io/idol_benchmark/GAP.render.html).

## About Mosek usage

Currently, both GLPK and [Mosek](https://www.mosek.com/) are installed on our automated 
GitHub Actions script. However, the Mosek solver is not currently in use for our benchmark
but is planned to be used in near future.
Note that the license is stored as a GitHub secret (with base64 encoding `cat mosek.lic | base64` to 
avoid corruption) and cannot publicly be accessed.
