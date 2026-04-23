# Generate Multiple Points from a Sobol Generator

Generates n consecutive points from the Sobol sequence and advances the
internal state of the generator.

## Usage

``` r
sobol_next_n(x, n, ...)
```

## Arguments

- x:

  A sobol_generator object created by
  [`sobol_generator`](https://alrobles.github.io/sobol/reference/sobol_generator.md)

- n:

  Integer, the number of points to generate. Must be non-negative.

- ...:

  Additional arguments (currently unused)

## Value

A numeric matrix with n rows and dimensions columns, where each row
represents a point in the Sobol sequence. Values are in \[0, 1). If n =
0, returns a 0 x dimensions matrix.

## Examples

``` r
if (FALSE) { # \dontrun{
gen <- sobol_generator(dimensions = 2)
points <- sobol_next_n(gen, n = 10)
print(dim(points)) # [1] 10  2
} # }
```
