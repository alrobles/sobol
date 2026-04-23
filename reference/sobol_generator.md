# Create a Sobol Sequence Generator

Creates an S3 object that wraps an Rcpp Sobol sequence generator for
incremental point generation. The generator maintains internal state and
allows for efficient generation of quasi-random sequences.

## Usage

``` r
sobol_generator(dimensions, skip = 0)
```

## Arguments

- dimensions:

  Integer, the number of dimensions for the Sobol sequence. Must be a
  positive integer.

- skip:

  Numeric, the number of initial points to skip (default: 0). This
  allows starting the sequence from any index for reproducibility.

## Value

An S3 object of class "sobol_generator" containing:

- generator:

  The underlying Rcpp reference class object

- dimensions:

  Number of dimensions

- initial_skip:

  Initial skip value used at construction

## Examples

``` r
if (FALSE) { # \dontrun{
# Create a 3-dimensional Sobol generator
gen <- sobol_generator(dimensions = 3)

# Generate a single point
point <- sobol_next(gen)

# Generate multiple points
points <- sobol_next_n(gen, n = 100)

# Skip to a specific index
sobol_skip_to(gen, 1000)

# Create a generator starting from index 50
gen2 <- sobol_generator(dimensions = 2, skip = 50)
} # }
```
