# sobol

Header-only C++17 Sobol sequence core library with precomputed direction numbers.

## Features

- Fast initialization using precomputed tables (1000 dimensions)
- Gray-code based incremental generation
- Joe-Kuo method with Property A enforcement
- Header-only, C++17 compatible
- Runtime generation fallback for dimensions > 1000

## Build and test

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
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

`include/sobol/r_api.hpp` provides `sobol::sobol_points_column_major(n, dimensions, skip)`
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
