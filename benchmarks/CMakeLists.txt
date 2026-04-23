# Google Benchmark integration for Sobol library
cmake_minimum_required(VERSION 3.16)

# Try to find Google Benchmark
find_package(benchmark QUIET)

if(benchmark_FOUND)
  message(STATUS "Found Google Benchmark, building benchmarks")

  # Core Sobol operations benchmark
  add_executable(bench_sobol_core bench_sobol_core.cpp)
  target_link_libraries(bench_sobol_core PRIVATE sobol benchmark::benchmark)
  target_compile_options(bench_sobol_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-O3 -march=native>
    $<$<CXX_COMPILER_ID:MSVC>:/O2>
  )

  # Memory and cache benchmark
  add_executable(bench_sobol_memory bench_sobol_memory.cpp)
  target_link_libraries(bench_sobol_memory PRIVATE sobol benchmark::benchmark)
  target_compile_options(bench_sobol_memory PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-O3 -march=native>
    $<$<CXX_COMPILER_ID:MSVC>:/O2>
  )

  # Scalability benchmark
  add_executable(bench_sobol_scalability bench_sobol_scalability.cpp)
  target_link_libraries(bench_sobol_scalability PRIVATE sobol benchmark::benchmark)
  target_compile_options(bench_sobol_scalability PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-O3 -march=native>
    $<$<CXX_COMPILER_ID:MSVC>:/O2>
  )

else()
  message(STATUS "Google Benchmark not found. Install it to build performance benchmarks:")
  message(STATUS "  Ubuntu/Debian: sudo apt-get install libbenchmark-dev")
  message(STATUS "  macOS: brew install google-benchmark")
  message(STATUS "  Or build from source: https://github.com/google/benchmark")
endif()
