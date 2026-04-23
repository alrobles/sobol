# Benchmark Suite: sobol vs pomp::sobol_design

This directory contains benchmark scripts for comparing the performance of the
`sobol` package against the reference implementation `pomp::sobol_design`.

## Goal

The primary goal is to demonstrate that the `sobol` package provides a faster,
more efficient implementation of Sobol sequence generation while maintaining
API compatibility with `pomp::sobol_design`.

## Scripts

### `compare_sobol_vs_pomp.R`

Main benchmark script that compares `sobol_design()` from this package against
`pomp::sobol_design()` across various parameter configurations.

**Usage:**

```r
# From R console
source("inst/benchmarks/compare_sobol_vs_pomp.R")

# From command line
Rscript inst/benchmarks/compare_sobol_vs_pomp.R
```

**Requirements:**

- `sobol` package installed
- `pomp` package installed
- `microbenchmark` package installed

Install requirements:

```r
# Install from CRAN
install.packages("pomp")
install.packages("microbenchmark")

# Install sobol package (from source)
devtools::install()
```

**Configuration:**

The benchmark tests a grid of configurations defined in the script:

- **Dimensions (`dims`)**: Number of parameters (5, 20, 100)
- **Sequence length (`nseq`)**: Number of points to generate (100, 1000)
- **Repetitions**: 10 runs per configuration for statistical robustness

You can modify the grid by editing the `grid` variable in the script:

```r
grid <- expand.grid(
  dims = c(5, 20, 100, 500),     # Add more dimensions
  nseq = c(100, 1000, 10000),    # Add more sequence lengths
  stringsAsFactors = FALSE
)
```

## Output

The benchmark produces:

1. **Console output** showing:
   - Progress for each configuration
   - Speedup summary table
   - Summary statistics

2. **CSV file** with detailed results (optional):
   - `benchmark_results_<timestamp>.csv` (non-interactive mode)
   - `benchmark_results.csv` (interactive mode, if user confirms)

### Example Output

```
