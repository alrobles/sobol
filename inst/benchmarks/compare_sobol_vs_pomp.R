# Benchmark comparison: sobol vs pomp::sobol_design
# This script compares the performance of the sobol package with pomp::sobol_design

library(microbenchmark)

# Try to load pomp if available
pomp_available <- requireNamespace("pomp", quietly = TRUE)

if (!pomp_available) {
  warning("pomp package not available. Install it with: install.packages('pomp')")
  warning("Running benchmarks for sobol package only.")
}

# Load sobol package (assumes it's installed or we're running from source)
if (!requireNamespace("sobol", quietly = TRUE)) {
  stop("sobol package must be installed or sourced")
}

library(sobol)

# Helper function to convert sobol_design-like parameters to sobol_points
sobol_wrapper <- function(lower, upper, nseq) {
  # Generate points in [0,1]^d
  dim <- length(lower)
  points <- sobol::sobol_points(n = nseq, dim = dim)

  # Scale to [lower, upper]
  for (i in seq_len(dim)) {
    points[, i] <- lower[i] + points[, i] * (upper[i] - lower[i])
  }

  colnames(points) <- names(lower)
  points
}

# Benchmark configurations
configs <- list(
  small_2d = list(
    lower = c(a = 0, b = 0),
    upper = c(a = 1, b = 1),
    nseq = 100
  ),
  medium_5d = list(
    lower = c(a = 0, b = 0, c = 0, d = 0, e = 0),
    upper = c(a = 1, b = 1, c = 1, d = 1, e = 1),
    nseq = 1000
  ),
  large_10d = list(
    lower = setNames(rep(0, 10), letters[1:10]),
    upper = setNames(rep(1, 10), letters[1:10]),
    nseq = 10000
  ),
  high_dim_50d = list(
    lower = setNames(rep(0, 50), paste0("x", 1:50)),
    upper = setNames(rep(1, 50), paste0("x", 1:50)),
    nseq = 1000
  ),
  extreme_100d = list(
    lower = setNames(rep(0, 100), paste0("x", 1:100)),
    upper = setNames(rep(1, 100), paste0("x", 1:100)),
    nseq = 500
  )
)

# Run benchmarks
results <- list()

for (config_name in names(configs)) {
  config <- configs[[config_name]]
  cat("\n=== Benchmark:", config_name, "===\n")
  cat("Dimensions:", length(config$lower), "\n")
  cat("Sequences:", config$nseq, "\n\n")

  if (pomp_available) {
    # Compare both implementations
    result <- microbenchmark(
      sobol = sobol_wrapper(config$lower, config$upper, config$nseq),
      pomp = pomp::sobol_design(
        lower = config$lower,
        upper = config$upper,
        nseq = config$nseq
      ),
      times = 20,
      unit = "ms"
    )
  } else {
    # Only benchmark sobol
    result <- microbenchmark(
      sobol = sobol_wrapper(config$lower, config$upper, config$nseq),
      times = 50,
      unit = "ms"
    )
  }

  print(result)
  results[[config_name]] <- result
}

# Summary statistics
cat("\n\n=== SUMMARY ===\n\n")

for (config_name in names(results)) {
  cat("Configuration:", config_name, "\n")
  result <- results[[config_name]]

  if (pomp_available) {
    sobol_times <- result$time[result$expr == "sobol"]
    pomp_times <- result$time[result$expr == "pomp"]

    sobol_median <- median(sobol_times) / 1e6  # Convert to ms
    pomp_median <- median(pomp_times) / 1e6
    speedup <- pomp_median / sobol_median

    cat(sprintf("  sobol median: %.3f ms\n", sobol_median))
    cat(sprintf("  pomp median:  %.3f ms\n", pomp_median))
    cat(sprintf("  Speedup:      %.2fx\n", speedup))
  } else {
    sobol_times <- result$time[result$expr == "sobol"]
    sobol_median <- median(sobol_times) / 1e6
    cat(sprintf("  sobol median: %.3f ms\n", sobol_median))
  }
  cat("\n")
}

# Save results for later analysis
saveRDS(results, file = "benchmark_results.rds")
cat("Results saved to: benchmark_results.rds\n")
