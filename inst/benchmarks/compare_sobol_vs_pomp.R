#!/usr/bin/env Rscript
# Benchmark: Compare sobol vs pomp::sobol_design
# =====================================================================
#
# This script benchmarks the sobol_design function from the sobol package
# against the reference implementation from pomp::sobol_design.
#
# Usage:
#   Rscript inst/benchmarks/compare_sobol_vs_pomp.R
#
# Requirements:
#   - sobol package installed
#   - pomp package installed
#   - microbenchmark package installed
#
# Output:
#   - Console output with benchmark results
#   - CSV file with detailed results (optional)

library(microbenchmark)
library(pomp)
library(sobol)

# Helper to create named bounds for a given dimension
create_bounds <- function(dim) {
  nms <- paste0("V", seq_len(dim))
  list(
    lower = setNames(rep(0, dim), nms),
    upper = setNames(rep(1, dim), nms)
  )
}

# Grid to test
# Testing various combinations of dimensions and sequence lengths
grid <- expand.grid(
  dims = c(5, 20, 100),
  nseq = c(100, 1000),
  stringsAsFactors = FALSE
)

# Store results
results <- data.frame()

cat("\n")
cat(strrep("=", 70), "\n")
cat("Benchmark: sobol vs pomp::sobol_design\n")
cat(strrep("=", 70), "\n")
cat(sprintf("Testing %d configurations\n", nrow(grid)))
cat(sprintf("Repetitions per configuration: 10\n"))
cat(strrep("=", 70), "\n\n")

for (i in seq_len(nrow(grid))) {
  d <- grid$dims[i]
  n <- grid$nseq[i]
  bounds <- create_bounds(d)

  cat(sprintf("[%d/%d] Benchmarking dims=%d, nseq=%d ... ",
              i, nrow(grid), d, n))

  # Run microbenchmark (10 repetitions for reasonable runtime)
  mb <- microbenchmark(
    sobol = sobol_design(
      lower = bounds$lower,
      upper = bounds$upper,
      nseq = n
    ),
    pomp = pomp::sobol_design(
      lower = bounds$lower,
      upper = bounds$upper,
      nseq = n
    ),
    times = 10
  )

  # Extract medians (microbenchmark object is a data.frame)
  med <- aggregate(time ~ expr, data = mb, FUN = median)
  med_sobol <- med$time[med$expr == "sobol"]
  med_pomp <- med$time[med$expr == "pomp"]

  speedup <- med_pomp / med_sobol
  faster <- ifelse(speedup > 1, "sobol", "pomp")
  label <- sprintf("%.2fx", abs(speedup))

  results <- rbind(results, data.frame(
    dims = d,
    nseq = n,
    sobol_ms = med_sobol / 1e6,   # nanoseconds to milliseconds
    pomp_ms = med_pomp / 1e6,
    speedup = speedup,
    speedup_label = label,
    faster = faster,
    stringsAsFactors = FALSE
  ))

  cat("done\n")
}

# Print speedup summary
cat("\n")
cat(strrep("=", 70), "\n")
cat("Speedup Summary (sobol vs pomp)\n")
cat(strrep("=", 70), "\n\n")

# Format results for display
display_results <- results
display_results$sobol_ms <- sprintf("%.3f", display_results$sobol_ms)
display_results$pomp_ms <- sprintf("%.3f", display_results$pomp_ms)
display_results$speedup <- NULL  # Remove numeric speedup, keep label

# Reorder columns for better readability
display_results <- display_results[, c(
  "dims", "nseq",
  "sobol_ms", "pomp_ms",
  "speedup_label", "faster"
)]

colnames(display_results) <- c(
  "Dims", "N_Seq",
  "Sobol (ms)", "Pomp (ms)",
  "Speedup", "Faster"
)

print(display_results, row.names = FALSE, right = FALSE)

cat("\n")
cat(strrep("=", 70), "\n")
cat("Summary Statistics\n")
cat(strrep("=", 70), "\n\n")

# Calculate overall statistics
sobol_wins <- sum(results$faster == "sobol")
pomp_wins <- sum(results$faster == "pomp")
avg_speedup <- mean(results$speedup)
median_speedup <- median(results$speedup)

cat(sprintf("Total configurations tested: %d\n", nrow(results)))
cat(sprintf("Configurations where sobol is faster: %d (%.1f%%)\n",
            sobol_wins, 100 * sobol_wins / nrow(results)))
cat(sprintf("Configurations where pomp is faster: %d (%.1f%%)\n",
            pomp_wins, 100 * pomp_wins / nrow(results)))
cat(sprintf("Average speedup (sobol/pomp): %.2fx\n", avg_speedup))
cat(sprintf("Median speedup (sobol/pomp): %.2fx\n", median_speedup))

if (avg_speedup > 1) {
  cat(sprintf(
    "\nSOBOL IS FASTER: On average, sobol is %.2fx faster than pomp\n",
    avg_speedup
  ))
} else {
  cat(sprintf(
    "\nPOMP IS FASTER: On average, pomp is %.2fx faster than sobol\n",
    1 / avg_speedup
  ))
}

cat("\n")
cat(strrep("=", 70), "\n\n")

# Optionally save results to CSV
output_file <- "benchmark_results.csv"
if (interactive()) {
  save_results <- readline(
    prompt = sprintf("Save results to %s? (y/n): ", output_file)
  )
  if (tolower(save_results) == "y") {
    write.csv(results, output_file, row.names = FALSE)
    cat(sprintf("Results saved to %s\n", output_file))
  }
} else {
  # In non-interactive mode, save automatically with timestamp
  timestamp <- format(Sys.time(), "%Y%m%d_%H%M%S")
  output_file <- sprintf("benchmark_results_%s.csv", timestamp)
  write.csv(results, output_file, row.names = FALSE)
  cat(sprintf("Results saved to %s\n", output_file))
}
