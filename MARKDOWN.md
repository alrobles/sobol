# Introduction

## Core design

The implementation is a modular header-only C++17 library:

- `include/sobol/primitive_polynomial.hpp`
  - Primitive polynomial discovery over GF(2) for requested dimensions.
- `include/sobol/direction_numbers.hpp`
  - Direction number table construction.
  - Joe-Kuo-style initialization with per-dimension full-rank checks for initial direction values.
- `include/sobol/sobol.hpp`
  - Gray-code incremental point generation (`next`) and direct `skip_to` logic.
- `include/sobol/r_api.hpp`
  - R-facing column-major adapter and incremental adapter for Rcpp wrappers.

## Algorithm notes

1. Primitive polynomials are generated from scratch by irreducibility + primitivity checks.
2. Direction numbers are built via standard recurrence with primitive polynomial taps.
3. Initial direction numbers are selected using GF(2) full-rank checks within each dimension.
4. Successive points are produced in O(dimensions) using the rightmost-zero-bit update rule.

# Mathematical Foundations

Use `<sobol/sobol.hpp>` for standalone C++ usage and `<sobol/r_api.hpp>` when exposing the
sequence through an Rcpp interface.
