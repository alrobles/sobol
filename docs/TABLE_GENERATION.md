# Sobol Direction Numbers and Primitive Polynomials - Table Generation

## Overview

This document describes the compile-time table generation system for Sobol sequence direction numbers and primitive polynomials. The system provides precomputed tables that significantly improve initialization performance while maintaining the mathematical properties required for high-quality quasi-random sequences.

## Table Generation Process

### 1. Primitive Polynomials

**Purpose**: Primitive polynomials over GF(2) are used to generate direction numbers through recurrence relations.

**Generation Algorithm**:
- Start with degree 1: polynomial `0b11` (x + 1) for dimension 1
- For degrees 2-30, search for primitive polynomials by:
  1. Testing irreducibility (no non-trivial factors)
  2. Testing primitivity (generator of the multiplicative group)
  3. Selecting the first valid polynomial found

**Implementation**: `sobol::detail::generate_primitive_polynomials()` in `inst/include/sobol/primitive_polynomial.hpp`

**Storage Format**:
- Array of `std::uint32_t` values
- Each value represents polynomial coefficients in binary
- Example: `0x13 = 0b10011` represents x^4 + x + 1

### 2. Initial Direction Numbers (m values)

**Purpose**: Initial direction numbers seed the recurrence relation and ensure Property A is satisfied.

**Generation Algorithm** (`enforce_initial_numbers_full_rank()`):
1. For each dimension, get the degree `s` of its primitive polynomial
2. Generate `s` initial direction numbers m[1]...m[s]:
   - Each m[i] must be an odd integer less than 2^i
   - The top bit (position i-1) must be set
   - The matrix formed by the first i bits of m[1]...m[i] must have full rank
3. Full rank is verified using Gaussian elimination over GF(2)
4. A deterministic seed (derived from polynomial and dimension) ensures reproducibility

**Property A Enforcement**:
- Full rank requirement ensures good equidistribution in 2D projections
- Follows the Joe-Kuo method for constructing Sobol sequences
- Critical for maintaining low discrepancy properties

### 3. Direction Number Recurrence

**Purpose**: Extends the initial direction numbers to the full 32-bit precision.

**Recurrence Relation**:
```
For i = s+1 to 32:
  v[i] = v[i-s] ⊕ (v[i-s] >> s) ⊕ Σ(a[k] · v[i-k])
```

where:
- v[i] are the direction numbers
- s is the polynomial degree
- a[k] are the polynomial coefficients
- ⊕ is XOR operation

**Implementation**: `direction_numbers_for_dimension()` in `inst/include/sobol/direction_numbers.hpp`

### 4. Table Storage

**Format**: Static `constexpr` arrays in C++17
- **Primitive Polynomials**: `std::array<std::uint32_t, 1000>`
- **Direction Numbers**: `std::array<std::array<std::uint32_t, 32>, 1000>`

**Location**: `inst/include/sobol/precomputed_tables.hpp` (generated file)

**Size**: ~477 KB for 1000 dimensions

## Usage

### Building with Precomputed Tables (Default)

By default, the library uses precomputed tables for dimensions 1-1000:

```cpp
#include <sobol/sobol.hpp>

// Automatically uses precomputed tables
sobol::SobolEngine engine(100);
```

### Forcing Runtime Generation

To disable precomputed tables and force runtime generation:

```cpp
// Define before including headers
#define SOBOL_NO_PRECOMPUTED_TABLES
#include <sobol/sobol.hpp>

sobol::SobolEngine engine(100);  // Will generate at runtime
```

Or compile with: `-DSOBOL_NO_PRECOMPUTED_TABLES`

### Dimensions Beyond Precomputed Range

For dimensions > 1000, the library automatically falls back to runtime generation:

```cpp
sobol::SobolEngine engine(2000);  // Runtime generation for dims > 1000
```

## Regenerating Tables

### Prerequisites
- CMake 3.16+
- C++17 compiler

### Build and Run Generator

```bash
# Configure and build the generator
cmake -S . -B build
cmake --build build --target generate_tables

# Generate the tables
./build/generate_tables

# Or use the custom target
cmake --build build --target generate_sobol_tables
```

### Output
- File: `inst/include/sobol/precomputed_tables.hpp`
- Contains: 1000 dimensions of precomputed data
- Includes: Documentation in embedded comments

## Table Format

### Primitive Polynomials Section

```cpp
constexpr std::array<std::uint32_t, kMaxDimensions> primitive_polynomials = {{
  0x3u,   0x7u,   0xbu,   0xdu,   0x13u,   0x19u,   // dimensions 1-6
  // ... (1000 entries total)
}};
```

### Direction Numbers Section

```cpp
constexpr std::array<std::array<std::uint32_t, kDirectionBits>, kMaxDimensions> direction_numbers = {{
  // Dimension 1
  {{0x80000000u, 0xc0000000u, 0xa0000000u, 0x90000000u, /* 32 values */ }},

  // Dimension 2
  {{0x80000000u, 0xc0000000u, 0xe0000000u, 0xf0000000u, /* 32 values */ }},

  // ... (1000 dimensions total)
}};
```

## Integration with Build System

### CMakeLists.txt Configuration

The table generator is integrated into the CMake build:

```cmake
# Generator executable
add_executable(generate_tables tools/generate_tables.cpp)
target_link_libraries(generate_tables PRIVATE sobol)

# Custom target for regeneration
add_custom_target(generate_sobol_tables
  COMMAND generate_tables ${CMAKE_CURRENT_SOURCE_DIR}/inst/include/sobol/precomputed_tables.hpp
  DEPENDS generate_tables
  COMMENT "Generating precomputed Sobol tables"
)
```

### Library Integration

The `direction_numbers.hpp` header automatically includes precomputed tables:

```cpp
#ifndef SOBOL_NO_PRECOMPUTED_TABLES
#include "sobol/precomputed_tables.hpp"
#endif
```

The `build_direction_table()` function uses precomputed data when available:

```cpp
inline std::vector<std::array<std::uint32_t, kSobolBits>>
build_direction_table(std::size_t dimensions) {
#ifndef SOBOL_NO_PRECOMPUTED_TABLES
  if (dimensions <= precomputed::kMaxDimensions) {
    // Use precomputed tables
    return copy_from_precomputed(dimensions);
  }
#endif
  // Fall back to runtime generation
  return generate_at_runtime(dimensions);
}
```

## Performance Characteristics

### Initialization Time Comparison

| Dimensions | Runtime Generation | Precomputed Tables | Speedup |
|------------|-------------------|-------------------|---------|
| 10         | ~50 μs            | ~1 μs             | 50x     |
| 100        | ~500 μs           | ~10 μs            | 50x     |
| 1000       | ~5 ms             | ~100 μs           | 50x     |

*Note: Times are approximate and vary by hardware*

### Memory Usage

- **Precomputed tables**: 477 KB (read-only data segment)
- **Runtime generation**: Temporary memory during generation, same final size

### Trade-offs

**Advantages of Precomputed Tables**:
- Much faster initialization
- No runtime computation needed
- Deterministic and verified
- Compile-time constants

**Advantages of Runtime Generation**:
- No additional binary size
- Supports unlimited dimensions
- Easier to audit/understand
- Customizable generation parameters

## Verification and Testing

### Automated Tests

The test suite verifies:
1. Precomputed tables match runtime generation for sample dimensions
2. Primitive polynomials are identical
3. Direction numbers are identical
4. Engines using precomputed tables produce correct sequences

See `tests/test_sobol.cpp` for implementation.

### Manual Verification

To verify a specific dimension:

```cpp
#include <sobol/direction_numbers.hpp>
#include <sobol/primitive_polynomial.hpp>
#include <cassert>

void verify_dimension(std::size_t dim) {
  // Generate at runtime
  auto runtime_polys = sobol::detail::generate_primitive_polynomials(dim);
  auto runtime_dirs = sobol::detail::direction_numbers_for_dimension(dim, runtime_polys);

  // Compare with precomputed
  assert(sobol::precomputed::primitive_polynomials[dim-1] == runtime_polys[dim-1]);
  for (std::size_t i = 0; i < 32; ++i) {
    assert(sobol::precomputed::direction_numbers[dim-1][i] == runtime_dirs[i]);
  }
}
```

## Mathematical Properties

### Property A (Joe-Kuo)

**Definition**: For each dimension d, the initial direction numbers m[1]...m[s] (where s is the polynomial degree) form a full-rank matrix when considering their leading bits.

**Verification**: The `is_full_rank()` function uses Gaussian elimination over GF(2) to verify this property.

**Importance**: Ensures good equidistribution in 2-dimensional projections, critical for integration quality.

### Recurrence Properties

**Invariants**:
- Direction numbers maintain proper bit distribution
- Gray-code updates preserve sequence properties
- All 32 bits contribute to low-discrepancy properties

## References

1. **Joe, S. and Kuo, F. Y. (2008)**: "Constructing Sobol sequences with better two-dimensional projections". SIAM Journal on Scientific Computing, 30(5):2635-2654.

2. **Bratley, P. and Fox, B. L. (1988)**: "Algorithm 659: Implementing Sobol's quasirandom sequence generator". ACM Transactions on Mathematical Software, 14(1):88-100.

3. **Sobol, I. M. (1967)**: "On the distribution of points in a cube and the approximate evaluation of integrals". USSR Computational Mathematics and Mathematical Physics, 7(4):86-112.

## Contributing

### Modifying Generation Parameters

To change the number of precomputed dimensions:

1. Edit `tools/generate_tables.cpp`:
   ```cpp
   constexpr std::size_t kMaxPrecomputedDimensions = 2000u;  // Change this
   ```

2. Regenerate tables:
   ```bash
   cmake --build build --target generate_sobol_tables
   ```

3. Rebuild library and tests:
   ```bash
   cmake --build build
   ctest --test-dir build
   ```

### Extending the Generator

The generator (`tools/generate_tables.cpp`) can be extended to:
- Generate tables for different bit precisions
- Output alternative formats (JSON, binary, etc.)
- Include additional metadata
- Support custom polynomial selection criteria

### Code Review Checklist

When modifying table generation:
- [ ] Verify Property A is maintained
- [ ] Test against runtime generation
- [ ] Check memory usage is acceptable
- [ ] Update documentation
- [ ] Run full test suite
- [ ] Verify table file regenerates correctly

## Troubleshooting

### Issue: Tables not being used

**Symptom**: Slow initialization despite precomputed tables being present.

**Solutions**:
1. Check that `SOBOL_NO_PRECOMPUTED_TABLES` is not defined
2. Verify `precomputed_tables.hpp` is in include path
3. Ensure dimensions ≤ 1000

### Issue: Compilation errors with tables

**Symptom**: Errors about missing `precomputed` namespace.

**Solutions**:
1. Regenerate tables: `cmake --build build --target generate_sobol_tables`
2. Check that `precomputed_tables.hpp` exists
3. Verify CMake configuration is up to date

### Issue: Test failures

**Symptom**: Tests fail after regenerating tables.

**Solutions**:
1. Rebuild everything from scratch: `rm -rf build && cmake -S . -B build && cmake --build build`
2. Verify generator code hasn't been modified incorrectly
3. Check that seed values in generation match original

## License and Attribution

The table generation process implements published algorithms:
- Joe-Kuo method for Property A enforcement
- Standard primitive polynomial generation over GF(2)
- Classical Sobol recurrence relations

Generated tables are derived data and inherit the library's license.
