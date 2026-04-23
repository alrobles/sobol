# Generate a Batch of Sobol Points

Efficiently generates a matrix of Sobol sequence points. This is a
convenience function that does not maintain state between calls. For
incremental generation, use
[`sobol_generator`](https://alrobles.github.io/sobol/reference/sobol_generator.md)
instead.

## Arguments

- n:

  Integer, the number of points to generate. Must be non-negative.

- dim:

  Integer, the number of dimensions for each point. Must be a positive
  integer.

- skip:

  Numeric, the number of initial points to skip (default: 0). This
  allows generating subsequences of the Sobol sequence.

## Value

A numeric matrix with n rows and dim columns, where each row represents
a point in the Sobol sequence. Values are in \[0, 1). If n = 0, returns
a 0 x dim matrix.

## Details

This function is implemented directly in C++ via Rcpp and is more
efficient than creating a generator and calling `sobol_next_n` when you
know in advance how many points you need.

The skip parameter allows you to start from any point in the sequence,
which is useful for:

- Reproducibility: generating the same subsequence across runs

- Parallelization: different workers can generate non-overlapping
  segments

- Continuation: extending a previous sequence without regenerating early
  points

## See also

[`sobol_generator`](https://alrobles.github.io/sobol/reference/sobol_generator.md)
for incremental generation with state

## Examples

``` r
if (FALSE) { # \dontrun{
# Generate 1000 points in 5 dimensions
points <- sobol_points(n = 1000, dim = 5)
dim(points) # [1] 1000    5

# Skip the first 100 points
points_skipped <- sobol_points(n = 100, dim = 2, skip = 100)

# Empty result
empty <- sobol_points(n = 0, dim = 3)
dim(empty) # [1] 0 3
} # }
```
