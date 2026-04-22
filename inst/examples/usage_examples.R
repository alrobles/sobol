# Example Usage of Sobol Sequence Generator
# ==========================================

library(sobol)

# Example 1: Basic batch generation
# ----------------------------------
cat("Example 1: Batch generation\n")
cat("============================\n\n")

# Generate 10 points in 2 dimensions
points <- sobol_points(n = 10, dimensions = 2)
cat("Generated 10 points in 2 dimensions:\n")
print(head(points))
cat(sprintf("\nDimensions: %d x %d\n\n", nrow(points), ncol(points)))

# Example 2: Incremental generation with state
# ---------------------------------------------
cat("Example 2: Incremental generation\n")
cat("==================================\n\n")

# Create a generator for 3 dimensions
gen <- sobol_generator(dimensions = 3)
print(gen)
cat("\n")

# Generate points one at a time
cat("First point:\n")
print(sobol_next(gen))

cat("\nSecond point:\n")
print(sobol_next(gen))

cat("\nThird point:\n")
print(sobol_next(gen))

# Check current state
cat(sprintf("\nCurrent index: %g\n", sobol_index(gen)))
cat(sprintf("Dimensions: %d\n\n", sobol_dimensions(gen)))

# Example 3: Batch generation from a generator
# ---------------------------------------------
cat("Example 3: Batch from generator\n")
cat("================================\n\n")

gen2 <- sobol_generator(dimensions = 2)
batch <- sobol_next_n(gen2, n = 5)
cat("Generated 5 points at once:\n")
print(batch)
cat("\n")

# Example 4: Skip functionality
# ------------------------------
cat("Example 4: Skip to specific index\n")
cat("==================================\n\n")

# Create generator and skip to index 100
gen3 <- sobol_generator(dimensions = 2, skip = 100)
cat("Generator starting at index 100:\n")
print(gen3)
cat("\nFirst point (index 100):\n")
print(sobol_next(gen3))

# Skip to another index
sobol_skip_to(gen3, 200)
cat("\nAfter skipping to index 200:\n")
cat(sprintf("Current index: %g\n", sobol_index(gen3)))
cat("Next point (index 200):\n")
print(sobol_next(gen3))
cat("\n")

# Example 5: Summary method
# --------------------------
cat("Example 5: Summary information\n")
cat("==============================\n\n")

gen4 <- sobol_generator(dimensions = 5)
sobol_next_n(gen4, n = 50)
cat("After generating 50 points:\n")
print(summary(gen4))
cat("\n")

# Example 6: Reproducibility
# ---------------------------
cat("Example 6: Reproducibility\n")
cat("==========================\n\n")

# Two generators with the same parameters produce the same sequence
gen_a <- sobol_generator(dimensions = 2)
gen_b <- sobol_generator(dimensions = 2)

points_a <- sobol_next_n(gen_a, n = 5)
points_b <- sobol_next_n(gen_b, n = 5)

cat("Generator A points:\n")
print(points_a)
cat("\nGenerator B points:\n")
print(points_b)
cat("\nAre they identical?", identical(points_a, points_b), "\n\n")

# Example 7: Comparison between batch and incremental
# ----------------------------------------------------
cat("Example 7: Batch vs Incremental\n")
cat("================================\n\n")

# Batch generation
batch_result <- sobol_points(n = 3, dimensions = 2)
cat("Batch generation (first 3 points):\n")
print(batch_result)

# Incremental generation
gen_inc <- sobol_generator(dimensions = 2)
inc_result <- sobol_next_n(gen_inc, n = 3)
cat("\nIncremental generation (first 3 points):\n")
print(inc_result)

cat("\nAre they identical?", identical(batch_result, inc_result), "\n\n")

cat("All examples completed successfully!\n")
