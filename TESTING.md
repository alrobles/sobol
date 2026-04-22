# Testing Strategy for Sobol Sequence Generator

This document describes the comprehensive testing strategy for the Sobol sequence generator library, covering both C++ and R implementations.

## Overview

The testing infrastructure ensures mathematical correctness, sequence validity, API usability, and edge case handling across all supported platforms and use cases.

## C++ Testing

### Test Suites

#### 1. Basic Test Suite (`tests/test_sobol.cpp`)
Core functionality tests covering:
- Primitive polynomial generation
- Single-dimension sequence validation
- Skip functionality
- Direction table building
- Matrix conversion (row-major to column-major)
- RGeneratorAdapter interface
- Basic error handling
- Precomputed tables validation

**Run:** `ctest --test-dir build --output-on-failure`

#### 2. Enhanced Test Suite (`tests/test_sobol_enhanced.cpp`)
Comprehensive test suite covering:

##### Property A Tests
- Validates that initial direction numbers form full-rank matrices
- Tests for dimensions 1-10
- Ensures mathematical correctness of Joe-Kuo method

##### Mathematical Correctness Tests
- 1D reference values (8 points): `[0.0, 0.5, 0.75, 0.25, 0.375, 0.875, 0.625, 0.125]`
- 2D reference values (4 points)
- 3D reference values (8 points)
- Basic validation for dimensions 4-10

##### Sequence Validity Tests
- Uniformity: Verifies points are well-distributed across bins
- Uniqueness: Ensures no duplicate points in first N points
- Gray code correctness: Compares incremental vs skip_to generation

##### Corner Cases and Boundary Conditions
- Zero skip
- Single dimension
- Large dimensions (100, 1000)
- Large skip values (1,000,000+)
- Skip backwards
- Maximum supported index (2^32 - 1)
- Invalid inputs (zero dimensions, skip beyond max)
- Matrix size overflow

##### R API Tests
- Column-major conversion correctness
- RGeneratorAdapter equivalence with SobolEngine
- Empty batch handling
- Matrix size overflow protection

##### Primitive Polynomial Tests
- Correctness of generated polynomials
- Known polynomial values verification
- Polynomial degree calculation

##### Precomputed Tables Tests
- Runtime vs precomputed comparison for first 10 dimensions
- Verification that precomputed tables are used for ≤1000 dimensions
- Can be disabled with `SOBOL_NO_PRECOMPUTED_TABLES`

### Building and Running C++ Tests

```bash
# Standard build
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure

# Test without precomputed tables
cmake -S . -B build -DCMAKE_CXX_FLAGS="-DSOBOL_NO_PRECOMPUTED_TABLES"
cmake --build build
ctest --test-dir build --output-on-failure

# Test with code coverage
cmake -S . -B build \
  -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
lcov --capture --directory build --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info
lcov --list coverage.info
```

## R Testing

### Test Suites

#### 1. Basic Test Suite (`tests/testthat/test-sobol_generator.R`)
Core R API tests covering:
- S3 object creation and structure
- Parameter validation (dimensions, skip)
- Point generation (single and batch)
- State advancement
- Skip functionality
- Query functions (index, dimensions)
- S3 methods (print, summary)
- Reproducibility
- Edge cases (large dimensions, large skip values)
- Sequence properties

**Run:** `Rscript -e "testthat::test_dir('tests/testthat')"`

#### 2. Enhanced Test Suite (`tests/testthat/test-sobol_enhanced.R`)
Comprehensive R tests covering:

##### Mathematical Correctness
- 1D, 2D, 3D reference value validation
- Sequence properties (first point all zeros, second point all 0.5s)
- Range validation ([0, 1))
- Low discrepancy verification

##### Consistency with C++ Backend
- R matches C++ for multiple dimensions (1-100)
- Skip_to correctness
- Batch vs incremental equivalence

##### API Usability
- Generator reusability after skip_to
- Independent generators
- Multiple API patterns

##### Edge Cases
- Very large dimensions (1000)
- Very large skip values (10,000,000)
- Invalid inputs
- Error handling

##### Performance and Scalability
- Large batch generation (10,000 points)
- Batch vs incremental performance

##### Additional Coverage
- Single dimension handling
- Matrix structure preservation

### Running R Tests

```bash
# Install package
R CMD INSTALL .

# Run tests
Rscript -e "testthat::test_dir('tests/testthat')"

# Run with coverage
Rscript -e "covr::package_coverage(type = 'tests', quiet = FALSE)"

# R CMD check
R CMD check --as-cran .
```

## Continuous Integration

### CI Workflows

#### C++ Tests (`.github/workflows/cpp-tests.yml`)
- **Platforms:** Ubuntu, macOS, Windows
- **Configurations:**
  - Standard build with precomputed tables
  - Build without precomputed tables
  - Code coverage on Ubuntu
- **Tests:**
  - All C++ test suites
  - Table generation verification
  - Coverage reporting to Codecov

#### R Package Tests (`.github/workflows/r-tests.yml`)
- **Platforms:** Ubuntu, macOS, Windows
- **R Versions:** 4.2, 4.3, 4.4
- **Tests:**
  - R CMD check
  - testthat test suites
  - Example code execution
  - Coverage reporting

### CI Triggers
- Push to `main`, `develop`, or `claude/**` branches
- Pull requests to `main` or `develop`
- Manual workflow dispatch

## Test Coverage Goals

### C++ Coverage
- **Core functionality:** 100%
- **Error handling:** 100%
- **Mathematical correctness:** Reference values for 1-10 dimensions
- **Edge cases:** All boundary conditions and corner cases
- **Platform compatibility:** Linux, macOS, Windows

### R Coverage
- **All exported functions:** 100%
- **S3 methods:** 100%
- **Error handling:** 100%
- **API usability:** All usage patterns
- **Integration with C++:** Consistency verification

## Extreme Case Testing

The test suites specifically cover extreme cases to ensure the library can serve as a transparent replacement for `sobol_design` in `pomp-explore`:

### Dimension Extremes
- ✅ Single dimension (1D)
- ✅ Standard dimensions (2-10D)
- ✅ Large dimensions (100D)
- ✅ Maximum precomputed dimensions (1000D)

### Index Extremes
- ✅ Zero skip (start from beginning)
- ✅ Large skip values (1,000,000+)
- ✅ Very large skip values (10,000,000+)
- ✅ Maximum supported index (2^32 - 1)
- ✅ Skip backwards (non-sequential access)

### Batch Size Extremes
- ✅ Empty batches (n=0)
- ✅ Single point batches (n=1)
- ✅ Standard batches (n=10-100)
- ✅ Large batches (n=10,000)

### Numerical Extremes
- ✅ Points at maximum index (boundary of 32-bit direction numbers)
- ✅ Overflow detection and prevention
- ✅ Matrix size overflow protection

## Compile-Time Testing

### Precomputed Tables
The library includes precomputed tables for 1000 dimensions, generated at compile time:

```bash
# Generate tables
cmake --build build --target generate_sobol_tables
```

**Verification:**
- Tables are generated by `tools/generate_tables.cpp`
- Output written to `include/sobol/precomputed_tables.hpp`
- Tests compare precomputed vs runtime generation
- Ensures consistency and correctness

**Testing the generated tables:**
1. Build with precomputed tables (default)
2. Build without precomputed tables (`-DSOBOL_NO_PRECOMPUTED_TABLES`)
3. Compare results - should be identical
4. Verify performance improvement (~50x faster initialization)

## Property A Validation

Property A (Joe-Kuo requirement) ensures the quality of the Sobol sequence:

**Test:** `test_property_a_all_dimensions()`
- Validates initial direction numbers form full-rank matrices
- Tests dimensions 1-10
- Ensures each dimension's initial numbers are linearly independent
- Critical for low-discrepancy properties

## Adding New Tests

### C++ Tests
1. Add test function to `tests/test_sobol_enhanced.cpp`
2. Follow naming convention: `test_<category>_<specific_test>()`
3. Call from `main()` function
4. Use `assert()` for validation
5. Rebuild and run: `cmake --build build && ctest --test-dir build`

### R Tests
1. Add test to `tests/testthat/test-sobol_enhanced.R`
2. Use `test_that()` blocks
3. Follow testthat expectations: `expect_*`
4. Run: `Rscript -e "testthat::test_dir('tests/testthat')"`

## Test Maintenance

### When to Update Tests
- Adding new features
- Modifying algorithms
- Changing API
- Fixing bugs
- Adding platforms or R versions

### Test Review Checklist
- [ ] All tests pass on all platforms
- [ ] Coverage meets goals (>95%)
- [ ] Edge cases covered
- [ ] Documentation updated
- [ ] CI workflows pass
- [ ] Performance regressions checked

## Known Limitations

1. **Maximum dimensions:** Limited by available primitive polynomials (currently supports 1000+ dimensions)
2. **Maximum index:** 2^32 - 1 (32-bit direction numbers)
3. **Platform-specific:** Some tests may need adjustment for unusual platforms
4. **R version:** Requires R >= 3.5.0, testthat >= 3.0.0

## References

- Joe, S., & Kuo, F. Y. (2008). Constructing Sobol sequences with better two-dimensional projections. SIAM Journal on Scientific Computing, 30(5), 2635-2654.
- Bratley, P., & Fox, B. L. (1988). Algorithm 659: Implementing Sobol's quasirandom sequence generator. ACM Transactions on Mathematical Software, 14(1), 88-100.

## Contact

For questions or issues related to testing:
- Open an issue on GitHub
- Check the README.md for general documentation
- See GETTING_STARTED.md for usage examples
