# Benchmarking idol's Branch-and-Price - Generalized Assignment Problem

![Maintained](https://img.shields.io/maintenance/yes/2023)
![GitHub issues](https://img.shields.io/github/issues-raw/hlefebvr/idol-benchmark-gap)
![GitHub Workflow Status (branch)](https://img.shields.io/github/actions/workflow/status/hlefebvr/idol-benchmark-gap/benchmark.yml?branch=main)

This code contains the code for benchmarking idol's Branch-and-Price implementation.
Currently, the comparison is done between [idol (C++ library)](https://github.com/hlefebvr/idol) and [Coluna.jl (julia package)](https://github.com/atoptima/Coluna.jl). 

The external solver used to solve every
involved sub-problem is [GLPK](https://www.gnu.org/software/glpk/).

## Results

- [See the full report](https://hlefebvr.github.io/idol-benchmark-gap/GAP.render.html).
