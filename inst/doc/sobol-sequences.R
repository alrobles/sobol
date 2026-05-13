## ----setup, include = FALSE---------------------------------------------------
knitr::opts_chunk$set(
  collapse = TRUE,
  comment = "#>",
  fig.width = 7,
  fig.height = 5
)

## ----basic_example------------------------------------------------------------
library(sobol)

# Create a 3-dimensional Sobol generator
gen <- sobol_generator(dimensions = 3)

# Generate a single point
point <- sobol_next(gen)
print(point)

# Generate multiple points
points <- sobol_next_n(gen, n = 5)
print(points)

## ----incremental--------------------------------------------------------------
# Create a generator
gen <- sobol_generator(dimensions = 2)

# Generate points one at a time
for (i in 1:5) {
  point <- sobol_next(gen)
  cat("Point", i, ":", point, "\n")
}

# Check current index
current_idx <- sobol_index(gen)
cat("Current index:", current_idx, "\n")

## ----skip_ahead---------------------------------------------------------------
# Create a generator starting from index 100
gen1 <- sobol_generator(dimensions = 2, skip = 100)
point1 <- sobol_next(gen1)

# Or skip to a specific index
gen2 <- sobol_generator(dimensions = 2)
sobol_skip_to(gen2, 100)
point2 <- sobol_next(gen2)

# These should be identical
print(all.equal(point1, point2))

## ----batch--------------------------------------------------------------------
# Generate 1000 points at once
gen <- sobol_generator(dimensions = 2)
points <- sobol_next_n(gen, n = 1000)

# Visualize the coverage
plot(points[, 1], points[, 2],
  pch = 20, cex = 0.5,
  main = "Sobol Sequence Coverage (2D)",
  xlab = "Dimension 1", ylab = "Dimension 2"
)

## ----comparison, fig.height=5, fig.width=10-----------------------------------
# Generate Sobol points
gen <- sobol_generator(dimensions = 2)
sobol_points <- sobol_next_n(gen, n = 1000)

# Generate random points
random_points <- matrix(runif(2000), ncol = 2)

# Plot comparison
par(mfrow = c(1, 2))
plot(sobol_points[, 1], sobol_points[, 2],
  pch = 20, cex = 0.5,
  main = "Sobol Sequence (n=1000)",
  xlab = "Dimension 1", ylab = "Dimension 2"
)

plot(random_points[, 1], random_points[, 2],
  pch = 20, cex = 0.5,
  main = "Random Sampling (n=1000)",
  xlab = "Dimension 1", ylab = "Dimension 2"
)
par(mfrow = c(1, 1))

## ----integration--------------------------------------------------------------
# Function to integrate: f(x, y) = x^2 + y^2 over [0,1]^2
# True value: 2/3
true_value <- 2 / 3

# Monte Carlo integration using random sampling
mc_random <- function(n) {
  points <- matrix(runif(2 * n), ncol = 2)
  mean(points[, 1]^2 + points[, 2]^2)
}

# Quasi-Monte Carlo integration using Sobol sequences
qmc_sobol <- function(n) {
  gen <- sobol_generator(dimensions = 2)
  points <- sobol_next_n(gen, n = n)
  mean(points[, 1]^2 + points[, 2]^2)
}

# Compare convergence
sample_sizes <- c(10, 50, 100, 500, 1000, 5000)
random_errors <- sapply(sample_sizes, function(n) abs(mc_random(n) - true_value))
sobol_errors <- sapply(sample_sizes, function(n) abs(qmc_sobol(n) - true_value))

# Plot convergence
plot(sample_sizes, random_errors,
  type = "b", log = "xy",
  col = "red", pch = 19,
  main = "Convergence Comparison",
  xlab = "Sample Size (n)", ylab = "Absolute Error",
  ylim = range(c(random_errors, sobol_errors))
)
lines(sample_sizes, sobol_errors, type = "b", col = "blue", pch = 19)
legend("topright", c("Random", "Sobol"),
  col = c("red", "blue"),
  pch = 19, lty = 1
)

## ----high_dim-----------------------------------------------------------------
# Generate points in 10 dimensions
gen_10d <- sobol_generator(dimensions = 10)
points_10d <- sobol_next_n(gen_10d, n = 1000)

# Check dimensions
cat("Generated", nrow(points_10d), "points in", ncol(points_10d), "dimensions\n")

# Examine coverage in first two dimensions
plot(points_10d[, 1], points_10d[, 2],
  pch = 20, cex = 0.5,
  main = "10D Sobol Sequence (Projection to 2D)",
  xlab = "Dimension 1", ylab = "Dimension 2"
)

## ----benchmark, eval=FALSE----------------------------------------------------
# library(microbenchmark)
# 
# # Benchmark batch generation
# microbenchmark(
#   n_100 = {
#     gen <- sobol_generator(dimensions = 10)
#     sobol_next_n(gen, 100)
#   },
#   n_1000 = {
#     gen <- sobol_generator(dimensions = 10)
#     sobol_next_n(gen, 1000)
#   },
#   n_10000 = {
#     gen <- sobol_generator(dimensions = 10)
#     sobol_next_n(gen, 10000)
#   },
#   times = 100
# )

## ----parallel_example, eval=FALSE---------------------------------------------
# library(parallel)
# 
# # Function to generate points from a specific range
# generate_chunk <- function(start_idx, n, dims) {
#   gen <- sobol_generator(dimensions = dims, skip = start_idx)
#   sobol_next_n(gen, n = n)
# }
# 
# # Generate 10,000 points in parallel chunks
# cl <- makeCluster(4)
# clusterEvalQ(cl, library(sobol))
# 
# chunks <- parLapply(cl, 0:3, function(i) {
#   generate_chunk(start_idx = i * 2500, n = 2500, dims = 5)
# })
# 
# stopCluster(cl)
# 
# # Combine results
# all_points <- do.call(rbind, chunks)
# cat("Generated", nrow(all_points), "points in parallel\n")

