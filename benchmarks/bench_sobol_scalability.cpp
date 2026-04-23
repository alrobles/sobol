// Benchmark suite for scalability testing
// This measures performance across different scales and scenarios

#include <benchmark/benchmark.h>
#include <sobol/sobol.hpp>
#include <thread>
#include <vector>

// Benchmark: Large batch generation
static void BM_LargeBatchGeneration(benchmark::State& state) {
  const std::size_t n = static_cast<std::size_t>(state.range(0));
  const std::size_t dimensions = 10;

  for (auto _ : state) {
    auto points = sobol::sobol_points(n, dimensions);
    benchmark::DoNotOptimize(points);
  }
  state.SetItemsProcessed(state.iterations() * n * dimensions);
}
BENCHMARK(BM_LargeBatchGeneration)
    ->RangeMultiplier(4)
    ->Range(1024, 1048576);

// Benchmark: High-dimensional generation
static void BM_HighDimensionalGeneration(benchmark::State& state) {
  const std::size_t dimensions = static_cast<std::size_t>(state.range(0));
  const std::size_t n = 1000;

  for (auto _ : state) {
    auto points = sobol::sobol_points(n, dimensions);
    benchmark::DoNotOptimize(points);
  }
  state.SetItemsProcessed(state.iterations() * n * dimensions);
  state.SetComplexityN(dimensions);
}
BENCHMARK(BM_HighDimensionalGeneration)
    ->RangeMultiplier(2)
    ->Range(10, 1000)
    ->Complexity();

// Benchmark: Incremental vs Batch (same total points)
static void BM_IncrementalGeneration(benchmark::State& state) {
  const std::size_t n = 10000;
  const std::size_t dimensions = 10;

  for (auto _ : state) {
    sobol::SobolEngine engine(dimensions);
    for (std::size_t i = 0; i < n; ++i) {
      auto point = engine.next();
      benchmark::DoNotOptimize(point);
    }
  }
  state.SetItemsProcessed(state.iterations() * n * dimensions);
}
BENCHMARK(BM_IncrementalGeneration);

static void BM_BatchGenerationComparison(benchmark::State& state) {
  const std::size_t n = 10000;
  const std::size_t dimensions = 10;

  for (auto _ : state) {
    auto points = sobol::sobol_points(n, dimensions);
    benchmark::DoNotOptimize(points);
  }
  state.SetItemsProcessed(state.iterations() * n * dimensions);
}
BENCHMARK(BM_BatchGenerationComparison);

// Benchmark: Sequential skip operations
static void BM_SequentialSkips(benchmark::State& state) {
  const std::size_t dimensions = 10;
  const std::size_t skip_stride = 1000;

  for (auto _ : state) {
    sobol::SobolEngine engine(dimensions);
    for (std::uint64_t i = 0; i < 10; ++i) {
      engine.skip_to(i * skip_stride);
      auto point = engine.next();
      benchmark::DoNotOptimize(point);
    }
  }
}
BENCHMARK(BM_SequentialSkips);

// Benchmark: Parallel engine creation (independent sequences)
static void BM_ParallelEngineCreation(benchmark::State& state) {
  const std::size_t dimensions = 10;
  const std::size_t num_engines = static_cast<std::size_t>(state.range(0));

  for (auto _ : state) {
    std::vector<std::thread> threads;
    threads.reserve(num_engines);

    for (std::size_t i = 0; i < num_engines; ++i) {
      threads.emplace_back([dimensions, i]() {
        sobol::SobolEngine engine(dimensions, i * 1000);
        auto point = engine.next();
        benchmark::DoNotOptimize(point);
      });
    }

    for (auto& thread : threads) {
      thread.join();
    }
  }
  state.SetComplexityN(num_engines);
}
BENCHMARK(BM_ParallelEngineCreation)
    ->RangeMultiplier(2)
    ->Range(1, std::thread::hardware_concurrency())
    ->Complexity()
    ->UseRealTime();

// Benchmark: Dimension scaling at different batch sizes
static void BM_DimensionScalingSmallBatch(benchmark::State& state) {
  const std::size_t dimensions = static_cast<std::size_t>(state.range(0));
  const std::size_t n = 100;

  for (auto _ : state) {
    auto points = sobol::sobol_points(n, dimensions);
    benchmark::DoNotOptimize(points);
  }
  state.SetComplexityN(dimensions);
}
BENCHMARK(BM_DimensionScalingSmallBatch)
    ->RangeMultiplier(2)
    ->Range(1, 1000)
    ->Complexity();

static void BM_DimensionScalingLargeBatch(benchmark::State& state) {
  const std::size_t dimensions = static_cast<std::size_t>(state.range(0));
  const std::size_t n = 10000;

  for (auto _ : state) {
    auto points = sobol::sobol_points(n, dimensions);
    benchmark::DoNotOptimize(points);
  }
  state.SetComplexityN(dimensions);
}
BENCHMARK(BM_DimensionScalingLargeBatch)
    ->RangeMultiplier(2)
    ->Range(1, 1000)
    ->Complexity();

// Benchmark: Throughput (points per second)
static void BM_Throughput(benchmark::State& state) {
  const std::size_t dimensions = 10;
  const std::size_t n = 10000;

  for (auto _ : state) {
    auto points = sobol::sobol_points(n, dimensions);
    benchmark::DoNotOptimize(points);
  }
  state.SetItemsProcessed(state.iterations() * n);
  state.SetLabel("points/sec");
}
BENCHMARK(BM_Throughput);

BENCHMARK_MAIN();
