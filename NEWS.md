# sobol 1.0.0

* Initial CRAN release.
* Core C++17 Sobol sequence engine with precomputed direction numbers
  for up to 1000 dimensions.
* `sobol_design()`: scaled parameter-space designs (API-compatible with
  `pomp::sobol_design()`).
* `sobol_points()`: batch generation of raw `[0, 1)` matrices.
* `sobol_generator()`: stateful S3 generator with `sobol_next()`,
  `sobol_next_n()`, `sobol_skip_to()`, `sobol_index()`, and
  `sobol_dimensions()`.
* Skip-ahead support for reproducible parallel workflows.
* Two vignettes: "Getting Started with sobol" and "Introduction to
  Sobol Sequences".
