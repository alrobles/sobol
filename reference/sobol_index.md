# Get Current Index of a Sobol Generator

Returns the current index of the generator, which is the index of the
next point that will be generated.

## Usage

``` r
sobol_index(x, ...)
```

## Arguments

- x:

  A sobol_generator object created by
  [`sobol_generator`](https://alrobles.github.io/sobol/reference/sobol_generator.md)

- ...:

  Additional arguments (currently unused)

## Value

A numeric value representing the current index (0-based).

## Examples

``` r
if (FALSE) { # \dontrun{
gen <- sobol_generator(dimensions = 2)
sobol_next(gen)
sobol_next(gen)
idx <- sobol_index(gen)
print(idx) # 2
} # }
```
