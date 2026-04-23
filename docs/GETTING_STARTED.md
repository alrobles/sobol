# Getting Started with the Sobol S3 Package

This guide helps you get started with the Sobol sequence generator R package.

## Installation

Since R is not available in the current environment, here are the typical installation instructions:

```r
# Install dependencies
install.packages(c("Rcpp", "testthat"))

# Install from source
# Option 1: Using devtools
devtools::install()

# Option 2: Using R CMD
# From terminal:
# R CMD INSTALL .

# Option 3: From GitHub (once pushed)
# devtools::install_github("alrobles/sobol")
```

## Quick Start

### 1. Load the Package

```r
library(sobol)
```

### 2. Basic Point Generation

The simplest way to generate Sobol points is using `sobol_points()`:

```r
# Generate 100 points in 5 dimensions
points <- sobol_points(n = 100, dimensions = 5)

# Examine the result
dim(points)    # [1] 100   5
head(points)   # First few points
range(points)  # All values are in [0, 1)
```

### 3. Incremental Generation

For more control, use a generator object:

```r
# Create a generator for 3 dimensions
gen <- sobol_generator(dimensions = 3)

# Generate one point at a time
p1 <- sobol_next(gen)
p2 <- sobol_next(gen)
p3 <- sobol_next(gen)

# Or generate a batch
batch <- sobol_next_n(gen, n = 10)
```

### 4. Check Generator State

```r
# Print quick summary
print(gen)

# Get detailed summary
summary(gen)

# Query specific properties
current_idx <- sobol_index(gen)      # Current position
dims <- sobol_dimensions(gen)         # Number of dimensions
```

## Common Use Cases

### Monte Carlo Integration

```r
# Define a function to integrate
f <- function(x) {
  # Example: f(x,y,z) = x^2 + y^2 + z^2
  sum(x^2)
}

# Generate Sobol points
n <- 10000
points <- sobol_points(n = n, dimensions = 3)

# Evaluate function at each point
results <- apply(points, 1, f)

# Compute integral estimate
integral <- mean(results)
print(paste("Integral estimate:", integral))
```

### Reproducible Sequences

```r
# Method 1: Start from a specific index
gen1 <- sobol_generator(dimensions = 2, skip = 1000)
points1 <- sobol_next_n(gen1, n = 100)

# Method 2: Skip dynamically
gen2 <- sobol_generator(dimensions = 2)
sobol_skip_to(gen2, 1000)
points2 <- sobol_next_n(gen2, n = 100)

# Both produce identical results
identical(points1, points2)  # TRUE
```

### Parallel Generation

```r
# Divide work among workers by skipping to different indices
generate_segment <- function(worker_id, segment_size, dimensions) {
  skip <- worker_id * segment_size
  gen <- sobol_generator(dimensions = dimensions, skip = skip)
  sobol_next_n(gen, n = segment_size)
}

# Worker 0: points 0-999
segment1 <- generate_segment(0, 1000, 3)

# Worker 1: points 1000-1999
segment2 <- generate_segment(1, 1000, 3)

# Worker 2: points 2000-2999
segment3 <- generate_segment(2, 1000, 3)

# Combine results
all_points <- rbind(segment1, segment2, segment3)
```

### Parameter Space Exploration

```r
# Generate uniform samples in [0,1]^d
n <- 1000
d <- 4
raw_points <- sobol_points(n = n, dimensions = d)

# Transform to actual parameter ranges
learning_rate <- raw_points[, 1] * 0.001 + 0.0001  # [0.0001, 0.0011]
momentum <- raw_points[, 2] * 0.9 + 0.1             # [0.1, 1.0]
dropout <- raw_points[, 3] * 0.5                     # [0, 0.5]
batch_size <- floor(raw_points[, 4] * 96) + 32      # [32, 128]

# Create parameter data frame
params <- data.frame(
  learning_rate = learning_rate,
  momentum = momentum,
  dropout = dropout,
  batch_size = batch_size
)

head(params)
```

## Working with the S3 Object

### Object Structure

A `sobol_generator` is an S3 object with three components:

```r
gen <- sobol_generator(dimensions = 3)
str(gen)
# List of 3
#  $ generator   : Rcpp Reference Class
#  $ dimensions  : int 3
#  $ initial_skip: num 0
#  - attr(*, "class")= chr "sobol_generator"
```

### Method Chaining

Since `sobol_skip_to()` returns the object invisibly, you can't directly chain it. But you can work around this:

```r
gen <- sobol_generator(dimensions = 2)
sobol_skip_to(gen, 100)
points <- sobol_next_n(gen, n = 10)

# Or in separate statements
gen <- sobol_generator(dimensions = 2)
invisible(sobol_skip_to(gen, 100))
points <- sobol_next_n(gen, n = 10)
```

### Custom Print Method

The package includes a helpful print method:

```r
gen <- sobol_generator(dimensions = 5, skip = 100)
print(gen)
# <Sobol Sequence Generator>
#   Dimensions: 5
#   Initial skip: 100
#   Current index: 100
#   Points generated: 0

sobol_next_n(gen, n = 50)
print(gen)
# <Sobol Sequence Generator>
#   Dimensions: 5
#   Initial skip: 100
#   Current index: 150
#   Points generated: 50
```

## Tips and Best Practices

### 1. Batch vs Incremental

- Use `sobol_points()` for one-time batch generation (more efficient)
- Use `sobol_generator()` when you need to:
  - Generate points incrementally
  - Track state between calls
  - Skip around the sequence
  - Implement custom iteration logic

### 2. Memory Efficiency

For very large point sets, generate in batches:

```r
gen <- sobol_generator(dimensions = 10)

# Generate 1 million points in chunks of 10k
n_total <- 1000000
chunk_size <- 10000
n_chunks <- n_total / chunk_size

for (i in 1:n_chunks) {
  chunk <- sobol_next_n(gen, n = chunk_size)
  # Process chunk...
  # (chunk will be garbage collected after each iteration)
}
```

### 3. Dimension Limits

The Sobol sequence works well up to hundreds of dimensions, but:
- Quality degrades for very high dimensions (>1000)
- Memory usage increases linearly with dimensions
- Consider dimension reduction techniques for high-dimensional problems

### 4. Index Arithmetic

Sobol sequences use 0-based indexing (like C++):
- Index 0 = first point
- Index 99 = 100th point
- After generating n points, index = n

```r
gen <- sobol_generator(dimensions = 2)
sobol_index(gen)  # 0

sobol_next(gen)
sobol_index(gen)  # 1

sobol_next_n(gen, n = 9)
sobol_index(gen)  # 10
```

### 5. Error Handling

The package validates all inputs:

```r
# These will throw helpful error messages:
sobol_generator(dimensions = -1)     # "dimensions must be positive"
sobol_generator(dimensions = 2.5)    # "dimensions must be an integer"
sobol_next_n(gen, n = -5)            # "n must be non-negative"
sobol_skip_to(gen, -10)              # "index must be non-negative"
```

## Running the Examples

The package includes comprehensive examples:

```r
# Load and run the example file
example_file <- system.file("examples", "usage_examples.R", package = "sobol")
source(example_file)
```

## Testing

Run the test suite:

```r
# Option 1: Using testthat
testthat::test_package("sobol")

# Option 2: Using devtools
devtools::test()

# Option 3: From command line
# R CMD check sobol_*.tar.gz
```

## Troubleshooting

### Package won't load

```r
# Check if Rcpp is installed
if (!require(Rcpp)) {
  install.packages("Rcpp")
}

# Rebuild the package
devtools::clean_dll()
devtools::load_all()
```

### "Module not found" error

Make sure the package is properly installed (not just loaded from source):

```r
# Install the package
devtools::install()

# Then load it
library(sobol)
```

### Points seem non-random

This is normal! Sobol sequences are *quasi-random*, not pseudo-random:
- They fill space more uniformly than random numbers
- Points follow a deterministic pattern
- This is intentional and beneficial for integration/sampling

## Getting Help

```r
# Function help
?sobol_generator
?sobol_next
?sobol_points

# Package help
?sobol
help(package = "sobol")

# Examples
example(sobol_generator)
```

## Next Steps

- Read the [R Package README](R_PACKAGE_README.md) for more details
- Review the [S3 Implementation](S3_IMPLEMENTATION.md) for architecture details
- Check out the [examples](inst/examples/usage_examples.R) for more use cases
- Explore the [test suite](tests/testthat/test-sobol_generator.R) for edge cases

## Further Reading

- Sobol, I.M. (1967). "On the distribution of points in a cube and the approximate evaluation of integrals"
- Joe, S. and Kuo, F. Y. (2008). "Constructing Sobol sequences with better two-dimensional projections"
- Wikipedia: [Sobol sequence](https://en.wikipedia.org/wiki/Sobol_sequence)
