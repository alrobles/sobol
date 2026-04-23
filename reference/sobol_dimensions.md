# Get Number of Dimensions of a Sobol Generator

Returns the number of dimensions configured for the generator.

## Usage

``` r
sobol_dimensions(x, ...)
```

## Arguments

- x:

  A sobol_generator object created by
  [`sobol_generator`](https://alrobles.github.io/sobol/reference/sobol_generator.md)

- ...:

  Additional arguments (currently unused)

## Value

An integer representing the number of dimensions.

## Examples

``` r
if (FALSE) { # \dontrun{
gen <- sobol_generator(dimensions = 5)
dims <- sobol_dimensions(gen)
print(dims) # 5
} # }
```
