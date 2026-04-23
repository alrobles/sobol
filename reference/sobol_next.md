# Generate the Next Point from a Sobol Generator

Generates a single point from the Sobol sequence and advances the
internal state of the generator.

## Usage

``` r
sobol_next(x, ...)
```

## Arguments

- x:

  A sobol_generator object created by
  [`sobol_generator`](https://alrobles.github.io/sobol/reference/sobol_generator.md)

- ...:

  Additional arguments (currently unused)

## Value

A numeric vector of length equal to the number of dimensions, containing
the next point in the Sobol sequence. Values are in \[0, 1).

## Examples

``` r
if (FALSE) { # \dontrun{
gen <- sobol_generator(dimensions = 3)
point <- sobol_next(gen)
print(point) # e.g., [0.5, 0.5, 0.5]
} # }
```
