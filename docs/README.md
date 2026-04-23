# sobol

[![C++ Tests](https://github.com/alrobles/sobol/actions/workflows/cpp-tests.yml/badge.svg)](https://github.com/alrobles/sobol/actions/workflows/cpp-tests.yml)
[![R Package Tests](https://github.com/alrobles/sobol/actions/workflows/r-tests.yml/badge.svg)](https://github.com/alrobles/sobol/actions/workflows/r-tests.yml)

Header-only C++17 Sobol sequence core library with precomputed direction numbers for quasi-Monte Carlo methods.

## Features

- Fast initialization using precomputed tables (1000 dimensions)
- Gray-code based incremental generation
- Joe-Kuo method with Property A enforcement
- Header-only, C++17 compatible
- Runtime generation fallback for dimensions > 1000
- **High-performance optimizations**: 5-15x faster than baseline implementations
- **Compiler intrinsics**: Hardware-accelerated bit operations
- **SIMD-friendly**: Auto-vectorization support for modern CPUs

## Performance

The library is heavily optimized for performance:
- Compiler intrinsics for trailing zero count (GCC/Clang/MSVC)
- Pointer arithmetic for better auto-vectorization
- Cache-friendly memory access patterns
- Aggressive compiler optimizations (-O3, -march=native)

See [OPTIMIZATION.md](OPTIMIZATION.md) for detailed performance documentation and benchmarking guidelines.

## Credits and References

This implementation is based on the work of:

- **Ilya M. Sobol** - Original Sobol sequence algorithm (1967)
- **Paul Bratley & Bennett L. Fox** - Algorithm implementation reference
  - Bratley, P., & Fox, B. L. (1988). "Algorithm 659: Implementing Sobol's quasirandom sequence generator." *ACM Transactions on Mathematical Software*, 14(1), 88-100. DOI: [10.1145/42288.214372](https://doi.org/10.1145/42288.214372)
- **Stephen Joe & Frances Y. Kuo** - Direction numbers and primitive polynomials
  - Joe, S., & Kuo, F. Y. (2008). "Constructing Sobol sequences with better two-dimensional projections." *SIAM Journal on Scientific Computing*, 30(5), 2635-2654. DOI: [10.1145/1358628.1358630](https://doi.org/10.1145/1358628.1358630)

## License

This package is distributed under the BSD 3-Clause License. See the LICENSE file for details.

## Build and test

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

For comprehensive testing documentation, see [TESTING.md](TESTING.md).

### Test Coverage

The library includes extensive test suites:

- **C++ Tests**: 28+ test functions covering Property A validation, mathematical correctness (1-10D), sequence validity, corner cases, and precomputed tables
- **R Tests**: 77 test cases covering API usability, edge cases, mathematical correctness, and consistency with C++ backend
- **CI/CD**: Automated testing on Ubuntu, macOS, and Windows via GitHub Actions
- **Extreme Cases**: Comprehensive testing for transparent replacement of `sobol_design` in `pomp-explore`

Run the test coverage summary:
```bash
bash scripts/test_coverage_summary.sh
```

## Standalone C++ API

```cpp
#include <sobol/sobol.hpp>

sobol::SobolEngine engine(/*dimensions=*/3, /*skip=*/0);
auto p0 = engine.next();
auto p1 = engine.next();

auto batch = sobol::sobol_points(/*n=*/1024, /*dimensions=*/3, /*skip=*/128);
```

- `SobolEngine(dimensions, skip)` initializes a Gray-code incremental generator.
- `next()` returns the current point and advances in O(dimensions).
- `skip_to(k)` jumps to the `k`-th point using Gray-code reconstruction.

## R integration API surface

`inst/include/sobol/r_api.hpp` provides `sobol::sobol_points_column_major(n, dimensions, skip)`
which returns flattened column-major doubles ready for an `Rcpp::NumericMatrix` bridge.
It also exposes `sobol::RGeneratorAdapter` for incremental generation (`next_point`,
`next_points_column_major`, `skip_to`).

`src/rcpp_interface.cpp` provides an Rcpp bridge:

- `sobol_points(n, dim, skip = 0)` returns an `n x dim` numeric matrix.
- `SobolGenerator` module class for incremental point generation from R.

Both interfaces validate arguments (`n`, `dim`, `skip`) and translate core errors into
R-friendly exceptions.

## Precomputed Tables

The library includes precomputed primitive polynomials and direction numbers for up to 1000 dimensions, providing significant performance improvements:

- **Initialization**: ~50x faster than runtime generation
- **Storage**: 477 KB of static constexpr data
- **Quality**: Joe-Kuo method with Property A enforcement
- **Fallback**: Automatic runtime generation for dimensions > 1000

See [TABLE_GENERATION.md](TABLE_GENERATION.md) for details on the table generation process, format, and how to regenerate tables.

To disable precomputed tables and force runtime generation, define `SOBOL_NO_PRECOMPUTED_TABLES` before including headers.
