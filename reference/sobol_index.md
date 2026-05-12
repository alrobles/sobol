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
# \donttest{
gen <- sobol_generator(dimensions = 2)
sobol_next(gen)
#> [1] 0 0
sobol_next(gen)
#> [1] 0.5 0.5
idx <- sobol_index(gen)
print(idx) # 2
#> [1] 2
# }
```
