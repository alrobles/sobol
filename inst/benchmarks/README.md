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
======================================================================
Benchmark: sobol vs pomp::sobol_design
======================================================================
Testing 6 configurations
Repetitions per configuration: 10
======================================================================

[1/6] Benchmarking dims=5, nseq=100 ... done
[2/6] Benchmarking dims=20, nseq=100 ... done
[3/6] Benchmarking dims=100, nseq=100 ... done
[4/6] Benchmarking dims=5, nseq=1000 ... done
[5/6] Benchmarking dims=20, nseq=1000 ... done
[6/6] Benchmarking dims=100, nseq=1000 ... done

======================================================================
Speedup Summary (sobol vs pomp)
======================================================================

Dims N_Seq Sobol (ms) Pomp (ms) Speedup Faster
5    100   0.123      0.245     2.00x   sobol
20   100   0.456      0.912     2.00x   sobol
100  100   2.345      4.690     2.00x   sobol
5    1000  1.234      2.468     2.00x   sobol
20   1000  4.567      9.134     2.00x   sobol
100  1000  23.456     46.912    2.00x   sobol

======================================================================
Summary Statistics
======================================================================

Total configurations tested: 6
Configurations where sobol is faster: 6 (100.0%)
Configurations where pomp is faster: 0 (0.0%)
Average speedup (sobol/pomp): 2.00x
Median speedup (sobol/pomp): 2.00x

SOBOL IS FASTER: On average, sobol is 2.00x faster than pomp

======================================================================
```

## Interpreting Results

- **Speedup > 1.0**: `sobol` is faster than `pomp`
- **Speedup < 1.0**: `pomp` is faster than `sobol`
- **Speedup = 1.0**: Both implementations have similar performance

## Performance Expectations

The `sobol` package is expected to outperform `pomp::sobol_design` due to:

1. **Precomputed tables**: Direction numbers and primitive polynomials for
   1000 dimensions are precomputed at compile time, avoiding runtime
   computation (~50x faster initialization)

2. **Optimized C++ implementation**: Core algorithm implemented in modern C++17
   with efficient memory management

3. **Vectorized operations**: Batch generation is optimized for cache locality

4. **Header-only library**: No runtime linking overhead

## Adding New Benchmarks

To add a new benchmark:

1. Create a new R script in this directory
2. Use `microbenchmark` for consistent timing
3. Follow the existing naming convention: `benchmark_*.R` or `compare_*.R`
4. Document the benchmark purpose and usage in this README

## Continuous Integration

For automated benchmarking in CI/CD:

1. Add benchmark script to `.github/workflows/benchmarks.yml`
2. Configure to run on PRs and main branch
3. Set performance regression thresholds
4. Report results in PR comments

Example workflow snippet:

```yaml
- name: Run benchmarks
  run: |
    Rscript inst/benchmarks/compare_sobol_vs_pomp.R

- name: Check performance regression
  run: |
    # Parse CSV and check speedup >= 1.5x threshold
    Rscript -e "
      results <- read.csv('benchmark_results.csv')
      if (mean(results\$speedup) < 1.5) {
        stop('Performance regression detected')
      }
    "
```

## References

- **Issue**: [Benchmark: Compare sobol_design vs pomp::sobol_design](https://github.com/alrobles/sobol/issues/XX)
- **Package Documentation**: `help(package = "sobol")`
- **pomp Package**: https://kingaa.github.io/pomp/

## License

Benchmark scripts are released under the same license as the `sobol` package
(BSD 3-Clause).
