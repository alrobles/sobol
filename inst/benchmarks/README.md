# Benchmark Infrastructure for Sobol Library

This directory contains benchmarking tools to measure and track performance improvements.

## C++ Benchmarks (Google Benchmark)

The C++ benchmarks require Google Benchmark to be installed:

**Installation:**
- Ubuntu/Debian: `sudo apt-get install libbenchmark-dev`
- macOS: `brew install google-benchmark`
- From source: https://github.com/google/benchmark

**Building:**
```bash
# Add benchmarks to CMake build
cmake -S . -B build
cmake --build build

# Run benchmarks
./build/bench_sobol_core
./build/bench_sobol_memory
./build/bench_sobol_scalability
```

**Benchmark Suites:**

1. **bench_sobol_core.cpp** - Core operations
   - Engine initialization
   - Single point generation
   - Batch point generation
   - Skip operations
   - Gray code conversion
   - Current point extraction

2. **bench_sobol_memory.cpp** - Memory and cache efficiency
   - Memory allocation overhead
   - Column-major vs row-major layouts
   - Direction table access patterns
   - Cache-friendly vs cache-unfriendly iteration
   - Memory footprint scaling

3. **bench_sobol_scalability.cpp** - Scalability testing
   - Large batch generation
   - High-dimensional generation
   - Incremental vs batch comparison
   - Parallel engine creation
   - Dimension scaling
   - Throughput measurements

## R Benchmarks

The R benchmarks use the `microbenchmark` package.

**Installation:**
```R
install.packages("microbenchmark")
install.packages("pomp")  # Optional, for comparison
```

**Running:**
```R
source("inst/benchmarks/compare_sobol_vs_pomp.R")
source("inst/benchmarks/profile_operations.R")
```

**R Benchmark Scripts:**

1. **compare_sobol_vs_pomp.R** - Compare with pomp::sobol_design
   - Tests various dimension and batch size combinations
   - Calculates speedup ratios
   - Saves results for analysis

2. **profile_operations.R** - Profile specific operations
   - Measures different operation patterns
   - Identifies bottlenecks
   - Tracks operations per second

## Interpreting Results

**Key Metrics:**
- **Throughput**: Points generated per second
- **Latency**: Time per operation
- **Scalability**: Performance vs dimension/batch size
- **Memory**: Bytes processed per second
- **Complexity**: Big-O notation (O(n), O(d), etc.)

**Expected Performance Characteristics:**
- Initialization: O(d) where d = dimensions
- Single point generation: O(d)
- Batch generation: O(n × d) where n = batch size
- Skip operation: O(d)

## Tracking Improvements

Before and after implementing optimizations:

1. Run all benchmarks and save results
2. Implement optimization
3. Re-run benchmarks
4. Compare results to quantify improvement
5. Document changes in optimization log

## Performance Targets

Based on the optimization roadmap:
- [ ] 2x speedup from compiler optimizations (-O3, -march=native)
- [ ] 1.5x speedup from SIMD/vectorization
- [ ] 1.3x speedup from memory layout optimizations
- [ ] 10x+ faster than pomp::sobol_design (baseline)

## Adding New Benchmarks

To add a new C++ benchmark:
1. Create a new .cpp file in benchmarks/
2. Add it to benchmarks/CMakeLists.txt
3. Follow Google Benchmark patterns

To add a new R benchmark:
1. Create a new .R file in inst/benchmarks/
2. Use microbenchmark for timing
3. Document the benchmark purpose
