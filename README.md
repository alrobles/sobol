# sobol

Header-only C++17 Sobol sequence core library.

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
