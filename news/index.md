# Changelog

## sobol 1.0.0

- Initial CRAN release.
- Core C++17 Sobol sequence engine with precomputed direction numbers
  for up to 1000 dimensions.
- [`sobol_design()`](https://alrobles.github.io/sobol/reference/sobol_design.md):
  scaled parameter-space designs (API-compatible with
  `pomp::sobol_design()`).
- [`sobol_points()`](https://alrobles.github.io/sobol/reference/sobol_points.md):
  batch generation of raw `[0, 1)` matrices.
- [`sobol_generator()`](https://alrobles.github.io/sobol/reference/sobol_generator.md):
  stateful S3 generator with
  [`sobol_next()`](https://alrobles.github.io/sobol/reference/sobol_next.md),
  [`sobol_next_n()`](https://alrobles.github.io/sobol/reference/sobol_next_n.md),
  [`sobol_skip_to()`](https://alrobles.github.io/sobol/reference/sobol_skip_to.md),
  [`sobol_index()`](https://alrobles.github.io/sobol/reference/sobol_index.md),
  and
  [`sobol_dimensions()`](https://alrobles.github.io/sobol/reference/sobol_dimensions.md).
- Skip-ahead support for reproducible parallel workflows.
- Two vignettes: “Getting Started with sobol” and “Introduction to Sobol
  Sequences”.
