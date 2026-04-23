# Performance Profiling Script for Sobol Library
# This script profiles specific operations to identify bottlenecks

library(sobol)

# Create a profiling function
profile_operation <- function(name, func, times = 100) {
  cat("\nProfiling:", name, "\n")

  # Warmup
  invisible(func())

  # Time execution
  start_time <- Sys.time()
  for (i in seq_len(times)) {
    invisible(func())
  }
  end_time <- Sys.time()

  elapsed <- as.numeric(end_time - start_time, units = "secs")
  avg_time <- elapsed / times

  cat(sprintf("  Total time: %.4f seconds\n", elapsed))
  cat(sprintf("  Average time per call: %.4f seconds\n", avg_time))
  cat(sprintf("  Operations per second: %.2f\n", times / elapsed))

  return(list(
    name = name,
    total_time = elapsed,
    avg_time = avg_time,
    ops_per_sec = times / elapsed
  ))
}

# Profile different operations
results <- list()

# 1. Small dimension, small batch
results$small_small <- profile_operation(
  "Small dim (2), small batch (100)",
  function() sobol_points(n = 100, dim = 2)
)

# 2. Small dimension, large batch
results$small_large <- profile_operation(
  "Small dim (2), large batch (100000)",
  function() sobol_points(n = 100000, dim = 2),
  times = 10
)

# 3. Medium dimension, medium batch
results$medium_medium <- profile_operation(
  "Medium dim (10), medium batch (10000)",
  function() sobol_points(n = 10000, dim = 10),
  times = 20
)

# 4. High dimension, small batch
results$high_small <- profile_operation(
  "High dim (100), small batch (1000)",
  function() sobol_points(n = 1000, dim = 100),
  times = 20
)

# 5. High dimension, medium batch
results$high_medium <- profile_operation(
  "High dim (100), medium batch (10000)",
  function() sobol_points(n = 10000, dim = 100),
  times = 5
)

# 6. Incremental generation
results$incremental <- profile_operation(
  "Incremental generation (10000 points, dim=10)",
  function() {
    gen <- sobol_generator(dim = 10)
    for (i in 1:10000) {
      gen$next()
    }
  },
  times = 5
)

# 7. Batch via generator
results$batch_gen <- profile_operation(
  "Batch via generator (10000 points, dim=10)",
  function() {
    gen <- sobol_generator(dim = 10)
    gen$next_n(10000)
  },
  times = 20
)

# 8. Skip operation
results$skip <- profile_operation(
  "Skip operation (skip to 1000000)",
  function() {
    gen <- sobol_generator(dim = 10)
    gen$skip_to(1000000)
  },
  times = 50
)

# Print summary
cat("\n\n=== PROFILING SUMMARY ===\n\n")
for (name in names(results)) {
  r <- results[[name]]
  cat(sprintf("%-40s: %8.4f sec (avg), %10.2f ops/sec\n",
    r$name, r$avg_time, r$ops_per_sec))
}

# Save profiling results
saveRDS(results, file = "profiling_results.rds")
cat("\nResults saved to: profiling_results.rds\n")
