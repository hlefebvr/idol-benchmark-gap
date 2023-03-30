# Benchmarking idol

## Github Actions

The benchmark is automatically done on GitHub Actions using the Mosek commercial solver. 
Note that the Mosek license is stored as a GitHub secret (with base64 encoding `cat mosek.lic | base64` to 
avoid corruption) and cannot publicly be accessed.
