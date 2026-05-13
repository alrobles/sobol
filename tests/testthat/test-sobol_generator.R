# =============================================================================
# Tests for sobol_generator, sobol_points, and related S3 methods
# Consolidated from test-sobol_generator.R and test-sobol_enhanced.R
# =============================================================================

# --- S3 Constructor and Object Creation --------------------------------------

test_that("sobol_generator creates valid S3 object", {
  gen <- sobol_generator(dimensions = 3)

  expect_s3_class(gen, "sobol_generator")
  expect_type(gen, "list")
  expect_named(gen, c("generator", "dimensions", "initial_skip"))
  expect_equal(gen$dimensions, 3L)
  expect_equal(gen$initial_skip, 0)
})

test_that("sobol_generator accepts skip parameter", {
  gen <- sobol_generator(dimensions = 2, skip = 100)

  expect_s3_class(gen, "sobol_generator")
  expect_equal(gen$initial_skip, 100)
  expect_equal(sobol_index(gen), 100)
})

test_that("sobol_generator validates dimensions", {
  expect_error(sobol_generator(dimensions = 0), "dimensions")
  expect_error(sobol_generator(dimensions = -1), "dimensions")
  expect_error(sobol_generator(dimensions = -5), "dimensions")
  expect_error(sobol_generator(dimensions = 1.5), "dimensions")
  expect_error(sobol_generator(dimensions = "3"), "dimensions")
  expect_error(sobol_generator(dimensions = NA))
  expect_error(sobol_generator(dimensions = NULL))
})

test_that("sobol_generator validates skip", {
  expect_error(sobol_generator(dimensions = 2, skip = -1), "skip")
  expect_error(sobol_generator(dimensions = 2, skip = 1.5), "skip")
  expect_error(sobol_generator(dimensions = 2, skip = "10"), "skip")
  expect_error(sobol_generator(dimensions = 2, skip = NA))
  expect_error(sobol_generator(dimensions = 2, skip = NULL))
})

# --- Point Generation --------------------------------------------------------

test_that("sobol_next generates single point", {
  gen <- sobol_generator(dimensions = 3)
  point <- sobol_next(gen)

  expect_type(point, "double")
  expect_length(point, 3)
  expect_true(all(point >= 0 & point < 1))
})

test_that("sobol_next advances state", {
  gen <- sobol_generator(dimensions = 2)

  expect_equal(sobol_index(gen), 0)
  sobol_next(gen)
  expect_equal(sobol_index(gen), 1)
  sobol_next(gen)
  expect_equal(sobol_index(gen), 2)
})

test_that("sobol_next validates input", {
  expect_error(sobol_next("not a generator"), "sobol_generator")
  expect_error(sobol_next(list(a = 1)), "sobol_generator")
})

test_that("sobol_next_n generates multiple points", {
  gen <- sobol_generator(dimensions = 2)
  points <- sobol_next_n(gen, n = 10)

  expect_type(points, "double")
  expect_true(is.matrix(points))
  expect_equal(dim(points), c(10, 2))
  expect_true(all(points >= 0 & points < 1))
})

test_that("sobol_next_n handles n=0", {
  gen <- sobol_generator(dimensions = 3)
  points <- sobol_next_n(gen, n = 0)

  expect_true(is.matrix(points))
  expect_equal(dim(points), c(0, 3))
})

test_that("sobol_next_n advances state correctly", {
  gen <- sobol_generator(dimensions = 2)

  expect_equal(sobol_index(gen), 0)
  sobol_next_n(gen, n = 5)
  expect_equal(sobol_index(gen), 5)
  sobol_next_n(gen, n = 3)
  expect_equal(sobol_index(gen), 8)
})

test_that("sobol_next_n validates inputs", {
  gen <- sobol_generator(dimensions = 2)

  expect_error(sobol_next_n("not a generator", 5), "sobol_generator")
  expect_error(sobol_next_n(gen, n = -1), "n")
  expect_error(sobol_next_n(gen, n = 1.5), "n")
  expect_error(sobol_next_n(gen, n = "5"), "n")
})

# --- Skip Functionality ------------------------------------------------------

test_that("sobol_skip_to changes state", {
  gen <- sobol_generator(dimensions = 2)

  expect_equal(sobol_index(gen), 0)
  sobol_skip_to(gen, 100)
  expect_equal(sobol_index(gen), 100)
  sobol_skip_to(gen, 50)
  expect_equal(sobol_index(gen), 50)
})

test_that("sobol_skip_to returns object invisibly", {
  gen <- sobol_generator(dimensions = 2)
  result <- withVisible(sobol_skip_to(gen, 10))

  expect_false(result$visible)
  expect_identical(result$value, gen)
})

test_that("sobol_skip_to validates inputs", {
  gen <- sobol_generator(dimensions = 2)

  expect_error(sobol_skip_to("not a generator", 10), "sobol_generator")
  expect_error(sobol_skip_to(gen, -1), "index")
  expect_error(sobol_skip_to(gen, "10"), "index")
})

test_that("generator can be reused after skip_to", {
  gen <- sobol_generator(dimensions = 2)

  points1 <- sobol_next_n(gen, n = 5)
  sobol_skip_to(gen, 0)
  points2 <- sobol_next_n(gen, n = 5)

  expect_identical(points1, points2)
})

# --- Query Functions ---------------------------------------------------------

test_that("sobol_index returns current index", {
  gen <- sobol_generator(dimensions = 2, skip = 50)

  expect_equal(sobol_index(gen), 50)
  sobol_next(gen)
  expect_equal(sobol_index(gen), 51)
})

test_that("sobol_dimensions returns dimensions", {
  gen1 <- sobol_generator(dimensions = 2)
  gen2 <- sobol_generator(dimensions = 5)

  expect_equal(sobol_dimensions(gen1), 2L)
  expect_equal(sobol_dimensions(gen2), 5L)
})

test_that("query functions validate input", {
  expect_error(sobol_index("not a generator"), "sobol_generator")
  expect_error(sobol_dimensions("not a generator"), "sobol_generator")
})

# --- Batch Function (sobol_points) -------------------------------------------

test_that("sobol_points generates correct dimensions", {
  points <- sobol_points(n = 10, dim = 3)

  expect_true(is.matrix(points))
  expect_equal(dim(points), c(10, 3))
  expect_true(all(points >= 0 & points < 1))
})

test_that("sobol_points handles skip parameter", {
  points1 <- sobol_points(n = 5, dim = 2, skip = 0)
  points2 <- sobol_points(n = 5, dim = 2, skip = 5)

  expect_false(identical(points1, points2))
})

test_that("sobol_points handles n=0", {
  points <- sobol_points(n = 0, dim = 3)

  expect_true(is.matrix(points))
  expect_equal(dim(points), c(0, 3))
})

test_that("sobol_points validates parameters", {
  expect_error(sobol_points(n = -1, dim = 2), "non-negative")
  expect_error(sobol_points(n = 10, dim = 0), "greater than zero")
})

test_that("works with single dimension", {
  gen <- sobol_generator(dimensions = 1)

  points <- sobol_next_n(gen, n = 10)
  expect_equal(dim(points), c(10, 1))
  expect_true(all(points >= 0 & points < 1))
})

# --- S3 Methods --------------------------------------------------------------

test_that("print.sobol_generator works", {
  gen <- sobol_generator(dimensions = 3, skip = 10)

  expect_output(print(gen), "Sobol Sequence Generator")
  expect_output(print(gen), "Dimensions: 3")
  expect_output(print(gen), "Initial skip: 10")
  expect_output(print(gen), "Current index: 10")

  result <- withVisible(print(gen))
  expect_false(result$visible)
  expect_identical(result$value, gen)
})

test_that("summary.sobol_generator creates summary object", {
  gen <- sobol_generator(dimensions = 4)
  sobol_next_n(gen, n = 20)

  summ <- summary(gen)

  expect_s3_class(summ, "summary.sobol_generator")
  expect_type(summ, "list")
  expect_named(summ, c("dimensions", "initial_skip", "current_index",
                        "points_generated"))
  expect_equal(summ$dimensions, 4L)
  expect_equal(summ$initial_skip, 0)
  expect_equal(summ$current_index, 20)
  expect_equal(summ$points_generated, 20)
})

test_that("print.summary.sobol_generator works", {
  gen <- sobol_generator(dimensions = 2, skip = 5)
  sobol_next_n(gen, n = 10)
  summ <- summary(gen)

  expect_output(print(summ), "Sobol Sequence Generator Summary")
  expect_output(print(summ), "Dimensions:\\s+2")
  expect_output(print(summ), "Initial skip:\\s+5")
  expect_output(print(summ), "Current index:\\s+15")
  expect_output(print(summ), "Points generated:\\s+10")

  result <- withVisible(print(summ))
  expect_false(result$visible)
  expect_identical(result$value, summ)
})

# --- Reproducibility ---------------------------------------------------------

test_that("generators with same parameters produce same sequence", {
  gen1 <- sobol_generator(dimensions = 3)
  gen2 <- sobol_generator(dimensions = 3)

  points1 <- sobol_next_n(gen1, n = 10)
  points2 <- sobol_next_n(gen2, n = 10)

  expect_identical(points1, points2)
})

test_that("skip produces consistent results", {
  gen1 <- sobol_generator(dimensions = 2)
  sobol_next_n(gen1, n = 10)
  points1 <- sobol_next_n(gen1, n = 5)

  gen2 <- sobol_generator(dimensions = 2, skip = 10)
  points2 <- sobol_next_n(gen2, n = 5)

  expect_identical(points1, points2)
})

test_that("batch and incremental produce same results", {
  batch <- sobol_points(n = 10, dim = 3)

  gen <- sobol_generator(dimensions = 3)
  incremental <- sobol_next_n(gen, n = 10)

  expect_identical(batch, incremental)
})

test_that("multiple generators are independent", {
  gen1 <- sobol_generator(dimensions = 2)
  gen2 <- sobol_generator(dimensions = 2)

  sobol_next_n(gen1, n = 10)

  expect_equal(sobol_index(gen2), 0)

  point1 <- sobol_next(gen1)
  point2 <- sobol_next(gen2)

  expect_false(identical(point1, point2))
})

# --- Mathematical Correctness (Reference Values) ----------------------------

test_that("1D sequence matches reference values", {
  gen <- sobol_generator(dimensions = 1)

  expected <- c(0.0, 0.5, 0.75, 0.25, 0.375, 0.875, 0.625, 0.125)

  for (i in seq_along(expected)) {
    point <- sobol_next(gen)
    expect_equal(point[1], expected[i], tolerance = 1e-10)
  }
})

test_that("2D sequence matches reference values", {
  gen <- sobol_generator(dimensions = 2)

  expected <- matrix(c(
    0.0, 0.0,
    0.5, 0.5,
    0.75, 0.25,
    0.25, 0.75
  ), ncol = 2, byrow = TRUE)

  for (i in seq_len(nrow(expected))) {
    point <- sobol_next(gen)
    expect_equal(point, expected[i, ], tolerance = 1e-10)
  }
})

test_that("3D sequence matches reference values", {
  gen <- sobol_generator(dimensions = 3)

  expected <- matrix(c(
    0.0, 0.0, 0.0,
    0.5, 0.5, 0.5,
    0.75, 0.25, 0.25,
    0.25, 0.75, 0.75,
    0.375, 0.625, 0.125,
    0.875, 0.125, 0.625,
    0.625, 0.875, 0.375,
    0.125, 0.375, 0.875
  ), ncol = 3, byrow = TRUE)

  for (i in seq_len(nrow(expected))) {
    point <- sobol_next(gen)
    expect_equal(point, expected[i, ], tolerance = 1e-10)
  }
})

# --- Sequence Properties -----------------------------------------------------

test_that("first point is all zeros", {
  for (dim in c(1, 2, 3, 5, 10)) {
    gen <- sobol_generator(dimensions = dim)
    first_point <- sobol_next(gen)
    expect_equal(first_point, rep(0, dim), tolerance = 1e-10)
  }
})

test_that("second point is all 0.5s", {
  for (dim in c(1, 2, 3, 5, 10)) {
    gen <- sobol_generator(dimensions = dim)
    sobol_next(gen)
    second_point <- sobol_next(gen)
    expect_equal(second_point, rep(0.5, dim), tolerance = 1e-10)
  }
})

test_that("all points are in [0, 1) range for 1000 points", {
  gen <- sobol_generator(dimensions = 5)
  points <- sobol_next_n(gen, n = 1000)

  expect_true(all(points >= 0))
  expect_true(all(points < 1))
})

test_that("sequence has low discrepancy", {
  gen <- sobol_generator(dimensions = 2)
  points <- sobol_next_n(gen, n = 1024)

  for (d in 1:2) {
    bins <- cut(points[, d], breaks = seq(0, 1, length.out = 9),
                include.lowest = TRUE)
    bin_counts <- table(bins)

    expect_true(all(bin_counts > 64))
    expect_true(all(bin_counts < 192))
  }
})

# --- Consistency: batch vs generator across dimensions -----------------------

test_that("batch and generator match across multiple dimensions", {
  for (dim in c(1, 2, 5, 10, 50, 100)) {
    gen <- sobol_generator(dimensions = dim)
    points_gen <- sobol_next_n(gen, n = 20)

    points_batch <- sobol_points(n = 20, dim = dim)

    expect_identical(points_gen, points_batch)
  }
})

test_that("skip_to matches sequential advancement", {
  gen1 <- sobol_generator(dimensions = 3)
  sobol_skip_to(gen1, 50)
  point1 <- sobol_next(gen1)

  gen2 <- sobol_generator(dimensions = 3)
  for (i in 1:50) {
    sobol_next(gen2)
  }
  point2 <- sobol_next(gen2)

  expect_equal(point1, point2, tolerance = 1e-10)
})

# --- Edge Cases and Scalability ----------------------------------------------

test_that("handles large dimensions (100)", {
  gen <- sobol_generator(dimensions = 100)

  point <- sobol_next(gen)
  expect_length(point, 100)
  expect_true(all(point >= 0 & point < 1))
})

test_that("handles very large dimensions (1000)", {
  gen <- sobol_generator(dimensions = 1000)

  point <- sobol_next(gen)
  expect_length(point, 1000)
  expect_true(all(point >= 0 & point < 1))
})

test_that("handles large skip values", {
  gen <- sobol_generator(dimensions = 2, skip = 1e6)

  expect_equal(sobol_index(gen), 1e6)
  point <- sobol_next(gen)
  expect_length(point, 2)
  expect_equal(sobol_index(gen), 1e6 + 1)
})

test_that("handles very large skip values (10M)", {
  gen <- sobol_generator(dimensions = 3, skip = 1e7)

  expect_equal(sobol_index(gen), 1e7)
  point <- sobol_next(gen)
  expect_length(point, 3)
  expect_true(all(point >= 0 & point < 1))
})

test_that("can generate large batches", {
  gen <- sobol_generator(dimensions = 10)

  points <- sobol_next_n(gen, n = 10000)

  expect_equal(nrow(points), 10000)
  expect_equal(ncol(points), 10)
})

test_that("batch matches incremental for large n", {
  dim <- 5
  n <- 1000

  batch <- sobol_points(n = n, dim = dim)

  gen <- sobol_generator(dimensions = dim)
  incremental <- sobol_next_n(gen, n = n)

  expect_identical(batch, incremental)
})
