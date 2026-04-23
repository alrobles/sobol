# Sobol Sequence Generator - R Package

An efficient R implementation of Sobol sequence generation for
quasi-Monte Carlo methods. This package provides both batch and
incremental interfaces with full support for reproducibility and state
management.

## Features

- **[`sobol_design()`](https://alrobles.github.io/sobol/reference/sobol_design.md)**:
  instant parameter grid with intuitive lower/upper bounds
- **Fast C++ engine**: core algorithms in C++17, exposed via Rcpp
- **Reproducibility**: deterministic sequences with skip‑ahead support
  for parallel workflows
- **Incremental Generation**: generate points one‑by‑one or in batches
- **Batch Generation**: Generate large matrices of points in a single
  call
- **Reproducibility**: Skip to arbitrary indices for reproducible
  subsequences
- **Full Documentation**: Comprehensive documentation with examples
- **Comprehensive test**: Test valiting mathematical correctness

## Installation

``` r
# Install from source
devtools::install_github("alrobles/sobol")
```

## Quick Start

The most common task is to explore a high‑dimensional parameter region
before running an optimiser. sobol_design() returns a data frame of
scaled points that you can feed directly to your objective function.

``` r
library(sobol)

# Define ranges for three fake hyperparameters
design <- sobol_design(
  lower = c(learning_rate = 0.0001, momentum = 0.00, dropout = 0.0),
  upper = c(learning_rate = 0.1000, momentum = 0.99, dropout = 0.5),
  nseq   = 200
)

head(design)
```

The design fills the space more evenly than random sampling – a key
advantage for global optimization.

### Going Deeper: Raw Points and Stateful Generation

sobol_design() is a convenience layer built on top of two lower‑level
backends. You can use them directly when you need raw (un‑scaled) points
or incremental control.

### Raw point matrices with sobol_points()

``` r
mat <- sobol_points(n = 512, dim = 4)   # 512×4 matrix in [0,1)
```

### Stateful generator with sobol_generator()

``` r
gen <- sobol_generator(dim = 2, skip = 1000)
pt  <- sobol_next(gen)                  # vector of length 2
batch <- sobol_next_n(gen, n = 100)     # 100×2 matrix

# Jump to any index (0‑based)
sobol_skip_to(gen, 2048)
sobol_index(gen)                        # current position
```

The generator is perfect for interleaved generation (generate a few
points, evaluate, generate more) and for parallel scouts that each
handle a non‑overlapping segment of the sequence.

### Reproducibility and Parallel Workflows

Sobol sequences are fully deterministic. Identical parameters always
yield identical points.

For parallel optimisation or multi‑node sampling, use skipping to
partition the sequence:

``` r
# Worker 1 handles points 0–999
w1 <- sobol_points(n = 1000, dim = 3, skip = 0)

# Worker 2 handles points 1000–1999
w2 <- sobol_points(n = 1000, dim = 3, skip = 1000)
```

No coordination between workers – just choose non‑overlapping skip
offsets.

### Performance

The library is heavily optimised:

- Compiler intrinsics for trailing‑zero count (GCC/Clang/MSVC)
- Column‑major layout for direct R matrix integration
- Even 1 million points in 10 dimensions execute in under a second on
  modern hardware.

### Reference at a glance

| Function                           | Purpose                                           | Output               |
|------------------------------------|---------------------------------------------------|----------------------|
| `sobol_design(lower, upper, nseq)` | Scaled parameter design (recommended entry point) | data frame           |
| `sobol_points(n, dim, skip = 0)`   | Raw Sobol matrix in \[0,1)                        | numeric matrix       |
| `sobol_generator(dim, skip = 0)`   | Stateful generator object                         | S3 `sobol_generator` |
| `sobol_next(x)`                    | Next single point                                 | numeric vector       |
| `sobol_next_n(x, n)`               | Next *n* points                                   | numeric matrix       |
| `sobol_skip_to(x, index)`          | Jump to index                                     | invisible(x)         |
| `sobol_index(x)`                   | Current index                                     | numeric              |
| `sobol_dimensions(x)`              | Number of dimensions                              | integer              |

``` r
a <- sobol_design(lower = c(p = 0), upper = c(p = 1), nseq = 64)
b <- sobol_design(lower = c(p = 0), upper = c(p = 1), nseq = 64)
identical(a, b)   # TRUE
```

Generate a matrix of Sobol points in one call:

``` r
library(sobol)

# Generate 1000 points in 5 dimensions
points <- sobol_points(n = 1000, dimensions = 5)
dim(points)  # [1] 1000    5

# All values are in [0, 1)
range(points)  # [1] 0.0000000 0.9999999
```

### Incremental Generation

Create a generator object for sequential point generation:

``` r
# Create a 3-dimensional generator
gen <- sobol_generator(dimensions = 3)
print(gen)
# <Sobol Sequence Generator>
#   Dimensions: 3
#   Initial skip: 0
#   Current index: 0
#   Points generated: 0

# Generate single points
point1 <- sobol_next(gen)
point2 <- sobol_next(gen)
point3 <- sobol_next(gen)

# Generate multiple points at once
batch <- sobol_next_n(gen, n = 100)
```

### Skip Functionality

Jump to specific indices in the sequence:

``` r
# Start from index 1000
gen <- sobol_generator(dimensions = 2, skip = 1000)

# Or skip later
gen <- sobol_generator(dimensions = 2)
sobol_skip_to(gen, 1000)
point <- sobol_next(gen)  # This is point #1000
```

### Query State

Check the current state of a generator:

``` r
gen <- sobol_generator(dimensions = 3)
sobol_next_n(gen, n = 50)

# Get current index
sobol_index(gen)  # 50

# Get dimensions
sobol_dimensions(gen)  # 3

# Get summary
summary(gen)
# Sobol Sequence Generator Summary
# ================================
# Dimensions:        3
# Initial skip:      0
# Current index:     50
# Points generated:  50
```

## S3 Methods

The package implements standard S3 methods for the `sobol_generator`
class:

- [`print()`](https://rdrr.io/r/base/print.html): Display generator
  state
- [`summary()`](https://rdrr.io/r/base/summary.html): Detailed summary
  of generator

## API Reference

### Constructor

- `sobol_generator(dimensions, skip = 0)`: Create a new Sobol generator

### Point Generation

- `sobol_next(x)`: Generate the next point
- `sobol_next_n(x, n)`: Generate n points
- `sobol_points(n, dimensions, skip = 0)`: Batch generation (stateless)

### State Management

- `sobol_skip_to(x, index)`: Jump to a specific index
- `sobol_index(x)`: Get current index
- `sobol_dimensions(x)`: Get number of dimensions

## Reproducibility

Sobol sequences are fully deterministic. Generators with the same
parameters will always produce identical sequences:

``` r
gen1 <- sobol_generator(dimensions = 2)
gen2 <- sobol_generator(dimensions = 2)

points1 <- sobol_next_n(gen1, n = 100)
points2 <- sobol_next_n(gen2, n = 100)

identical(points1, points2)  # TRUE
```

The skip functionality enables reproducible subsequences and parallel
generation:

``` r
# Generate points 100-199
worker1 <- sobol_generator(dimensions = 2, skip = 100)
batch1 <- sobol_next_n(worker1, n = 100)

# Generate points 200-299
worker2 <- sobol_generator(dimensions = 2, skip = 200)
batch2 <- sobol_next_n(worker2, n = 100)
```

## Use Cases

### Monte Carlo Integration

``` r
# Integrate a function over [0,1]^d
integrate_mc <- function(f, dimensions, n_points) {
  points <- sobol_points(n = n_points, dimensions = dimensions)
  mean(apply(points, 1, f))
}

# Example: integrate f(x,y) = x^2 + y^2
f <- function(p) p[1]^2 + p[2]^2
result <- integrate_mc(f, dimensions = 2, n_points = 10000)
```

### Parameter Space Exploration

``` r
# Generate parameter combinations for grid search
params <- sobol_points(n = 1000, dimensions = 3)

# Map to actual parameter ranges
learning_rate <- params[, 1] * 0.1  # [0, 0.1]
momentum <- params[, 2] * 0.9 + 0.1  # [0.1, 1.0]
dropout <- params[, 3] * 0.5  # [0, 0.5]
```

### Sensitivity Analysis

``` r
# Generate input samples for sensitivity analysis
gen <- sobol_generator(dimensions = 10)

# Generate primary samples
A <- sobol_next_n(gen, n = 1000)

# Generate secondary samples
B <- sobol_next_n(gen, n = 1000)
```

## Performance

The package uses a header-only C++ implementation with Rcpp for R
integration. Batch generation is highly optimized:

``` r
# Generate 1 million points in 10 dimensions
system.time(sobol_points(n = 1e6, dimensions = 10))
#   user  system elapsed
#  0.234   0.012   0.247
```

## Technical Details

- **Algorithm**: Implements the Sobol sequence generation algorithm with
  direction numbers
- **Precision**: Uses 64-bit arithmetic for index tracking
- **Memory**: Column-major layout for efficient R matrix operations
- **Thread Safety**: Each generator maintains independent state

## License

MIT License - see LICENSE file for details

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## References

- Sobol, I.M. (1967). “On the distribution of points in a cube and the
  approximate evaluation of integrals”
- Joe, S. and Kuo, F. Y. (2008). “Constructing Sobol sequences with
  better two-dimensional projections”

## See Also

- [Core C++ Library](https://github.com/alrobles/sobol) - The underlying
  C++ implementation
- Examples: `inst/examples/usage_examples.R`
