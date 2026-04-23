// Benchmark suite for core Sobol operations
// This measures the performance of basic SobolEngine operations

#include <benchmark/benchmark.h>
#include <sobol/sobol.hpp>

// Benchmark: Engine initialization across different dimensions
static void BM_EngineInit(benchmark::State& state) {
  const std::size_t dimensions = static_cast<std::size_t>(state.range(0));
  for (auto _ : state) {
    sobol::SobolEngine engine(dimensions);
    benchmark::DoNotOptimize(engine);
  }
  state.SetComplexityN(dimensions);
}
BENCHMARK(BM_EngineInit)
    ->RangeMultiplier(2)
    ->Range(1, 1000)
    ->Complexity();

// Benchmark: Single point generation
static void BM_SinglePointGeneration(benchmark::State& state) {
  const std::size_t dimensions = static_cast<std::size_t>(state.range(0));
  sobol::SobolEngine engine(dimensions);

  for (auto _ : state) {
    auto point = engine.next();
    benchmark::DoNotOptimize(point);
  }
  state.SetComplexityN(dimensions);
}
BENCHMARK(BM_SinglePointGeneration)
    ->RangeMultiplier(2)
    ->Range(1, 1000)
    ->Complexity();

// Benchmark: Batch point generation (hot loop)
static void BM_BatchPointGeneration(benchmark::State& state) {
  const std::size_t n = static_cast<std::size_t>(state.range(0));
  const std::size_t dimensions = 10;

  for (auto _ : state) {
    auto points = sobol::sobol_points(n, dimensions);
    benchmark::DoNotOptimize(points);
  }
  state.SetItemsProcessed(state.iterations() * n * dimensions);
  state.SetComplexityN(n);
}
BENCHMARK(BM_BatchPointGeneration)
    ->RangeMultiplier(2)
    ->Range(64, 65536)
    ->Complexity();

// Benchmark: Skip operation
static void BM_SkipTo(benchmark::State& state) {
  const std::size_t dimensions = 10;
  const std::uint64_t skip_index = static_cast<std::uint64_t>(state.range(0));

  for (auto _ : state) {
    sobol::SobolEngine engine(dimensions);
    engine.skip_to(skip_index);
    benchmark::DoNotOptimize(engine);
  }
  state.SetComplexityN(skip_index);
}
BENCHMARK(BM_SkipTo)
    ->RangeMultiplier(10)
    ->Range(100, 10000000)
    ->Complexity();

// Benchmark: Gray code computation (internal)
static void BM_GrayCodeConversion(benchmark::State& state) {
  std::uint64_t index = 0;
  for (auto _ : state) {
    std::uint64_t gray = index ^ (index >> 1);
    benchmark::DoNotOptimize(gray);
    ++index;
  }
}
BENCHMARK(BM_GrayCodeConversion);

// Benchmark: Current point extraction
static void BM_CurrentPoint(benchmark::State& state) {
  const std::size_t dimensions = static_cast<std::size_t>(state.range(0));
  sobol::SobolEngine engine(dimensions);
  engine.next();  // Advance to non-zero state

  for (auto _ : state) {
    auto point = engine.current_point();
    benchmark::DoNotOptimize(point);
  }
  state.SetComplexityN(dimensions);
}
BENCHMARK(BM_CurrentPoint)
    ->RangeMultiplier(2)
    ->Range(1, 1000)
    ->Complexity();

// Benchmark: Combined init + generation
static void BM_InitAndGenerate(benchmark::State& state) {
  const std::size_t dimensions = 10;
  const std::size_t n = static_cast<std::size_t>(state.range(0));

  for (auto _ : state) {
    sobol::SobolEngine engine(dimensions);
    for (std::size_t i = 0; i < n; ++i) {
      auto point = engine.next();
      benchmark::DoNotOptimize(point);
    }
  }
  state.SetItemsProcessed(state.iterations() * n);
}
BENCHMARK(BM_InitAndGenerate)
    ->RangeMultiplier(2)
    ->Range(1, 1024);

BENCHMARK_MAIN();
