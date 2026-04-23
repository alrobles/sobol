// Benchmark suite for memory and cache efficiency
// This measures memory access patterns and cache performance

#include <benchmark/benchmark.h>
#include <sobol/sobol.hpp>
#include <sobol/r_api.hpp>
#include <vector>
#include <numeric>

// Benchmark: Memory allocation overhead for batch generation
static void BM_BatchMemoryAllocation(benchmark::State& state) {
  const std::size_t n = static_cast<std::size_t>(state.range(0));
  const std::size_t dimensions = 10;

  for (auto _ : state) {
    std::vector<std::vector<double>> points;
    points.reserve(n);
    benchmark::DoNotOptimize(points);
  }
  state.SetBytesProcessed(state.iterations() * n * dimensions * sizeof(double));
}
BENCHMARK(BM_BatchMemoryAllocation)
    ->RangeMultiplier(2)
    ->Range(64, 65536);

// Benchmark: Column-major layout performance (R API)
static void BM_ColumnMajorGeneration(benchmark::State& state) {
  const std::size_t n = static_cast<std::size_t>(state.range(0));
  const std::size_t dimensions = 10;

  for (auto _ : state) {
    auto data = sobol::sobol_points_column_major(n, dimensions);
    benchmark::DoNotOptimize(data);
  }
  state.SetItemsProcessed(state.iterations() * n * dimensions);
  state.SetBytesProcessed(state.iterations() * n * dimensions * sizeof(double));
}
BENCHMARK(BM_ColumnMajorGeneration)
    ->RangeMultiplier(2)
    ->Range(64, 65536);

// Benchmark: Row-major vs Column-major comparison
static void BM_RowMajorGeneration(benchmark::State& state) {
  const std::size_t n = static_cast<std::size_t>(state.range(0));
  const std::size_t dimensions = 10;

  for (auto _ : state) {
    auto points = sobol::sobol_points(n, dimensions);
    benchmark::DoNotOptimize(points);
  }
  state.SetItemsProcessed(state.iterations() * n * dimensions);
  state.SetBytesProcessed(state.iterations() * n * dimensions * sizeof(double));
}
BENCHMARK(BM_RowMajorGeneration)
    ->RangeMultiplier(2)
    ->Range(64, 65536);

// Benchmark: Direction table access pattern
static void BM_DirectionTableAccess(benchmark::State& state) {
  const std::size_t dimensions = 100;
  sobol::SobolEngine engine(dimensions);

  // Warm up the cache
  for (int i = 0; i < 100; ++i) {
    engine.next();
  }

  for (auto _ : state) {
    auto point = engine.next();
    benchmark::DoNotOptimize(point);
  }
}
BENCHMARK(BM_DirectionTableAccess);

// Benchmark: State vector access
static void BM_StateVectorAccess(benchmark::State& state) {
  const std::size_t dimensions = static_cast<std::size_t>(state.range(0));
  std::vector<std::uint32_t> state_vec(dimensions, 0);

  for (auto _ : state) {
    std::uint32_t sum = 0;
    for (std::size_t d = 0; d < dimensions; ++d) {
      sum ^= state_vec[d];
    }
    benchmark::DoNotOptimize(sum);
  }
  state.SetComplexityN(dimensions);
}
BENCHMARK(BM_StateVectorAccess)
    ->RangeMultiplier(2)
    ->Range(1, 1000)
    ->Complexity();

// Benchmark: Cache-friendly iteration
static void BM_CacheFriendlyIteration(benchmark::State& state) {
  const std::size_t n = 1024;
  const std::size_t dimensions = 10;
  auto data = sobol::sobol_points_column_major(n, dimensions);

  for (auto _ : state) {
    double sum = 0.0;
    // Column-major access (cache-friendly)
    for (std::size_t col = 0; col < dimensions; ++col) {
      for (std::size_t row = 0; row < n; ++row) {
        sum += data[col * n + row];
      }
    }
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(BM_CacheFriendlyIteration);

// Benchmark: Cache-unfriendly iteration
static void BM_CacheUnfriendlyIteration(benchmark::State& state) {
  const std::size_t n = 1024;
  const std::size_t dimensions = 10;
  auto data = sobol::sobol_points_column_major(n, dimensions);

  for (auto _ : state) {
    double sum = 0.0;
    // Row-major access of column-major data (cache-unfriendly)
    for (std::size_t row = 0; row < n; ++row) {
      for (std::size_t col = 0; col < dimensions; ++col) {
        sum += data[col * n + row];
      }
    }
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(BM_CacheUnfriendlyIteration);

// Benchmark: Memory footprint for different dimensions
static void BM_MemoryFootprint(benchmark::State& state) {
  const std::size_t dimensions = static_cast<std::size_t>(state.range(0));

  for (auto _ : state) {
    state.PauseTiming();
    sobol::SobolEngine engine(dimensions);
    state.ResumeTiming();

    auto point = engine.next();
    benchmark::DoNotOptimize(point);
  }
}
BENCHMARK(BM_MemoryFootprint)
    ->RangeMultiplier(2)
    ->Range(1, 1000);

BENCHMARK_MAIN();
