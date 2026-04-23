test_that("sobol_design basic functionality works", {
  # Basic 2D design
  design <- sobol_design(
    lower = c(a = 0, b = 0),
    upper = c(a = 1, b = 1),
    nseq = 100
  )

  expect_s3_class(design, "data.frame")
  expect_equal(nrow(design), 100)
  expect_equal(ncol(design), 2)
  expect_equal(names(design), c("a", "b"))
  expect_true(all(design$a >= 0 & design$a <= 1))
  expect_true(all(design$b >= 0 & design$b <= 1))
})

test_that("sobol_design respects parameter ranges", {
  design <- sobol_design(
    lower = c(x = -10, y = 100),
    upper = c(x = 10, y = 200),
    nseq = 50
  )

  expect_true(all(design$x >= -10 & design$x <= 10))
  expect_true(all(design$y >= 100 & design$y <= 200))
})

test_that("sobol_design handles name ordering", {
  # Different order in lower and upper
  design <- sobol_design(
    lower = c(a = 0, b = 100),
    upper = c(b = 200, a = 1),
    nseq = 10
  )

  expect_equal(names(design), c("a", "b"))
  expect_true(all(design$a >= 0 & design$a <= 1))
  expect_true(all(design$b >= 100 & design$b <= 200))
})

test_that("sobol_design validates inputs", {
  # Length mismatch
  expect_error(
    sobol_design(lower = c(a = 0), upper = c(a = 1, b = 2), nseq = 10),
    "must have same length"
  )

  # Missing names
  expect_error(
    sobol_design(lower = c(0, 1), upper = c(1, 2), nseq = 10),
    "must be named vectors"
  )

  # Name mismatch
  expect_error(
    sobol_design(lower = c(a = 0, b = 1), upper = c(a = 1, c = 2), nseq = 10),
    "names.*must match"
  )

  # Invalid nseq
  expect_error(
    sobol_design(lower = c(a = 0), upper = c(a = 1), nseq = -1),
    "positive integer"
  )

  expect_error(
    sobol_design(lower = c(a = 0), upper = c(a = 1), nseq = 1.5),
    "positive integer"
  )

  # Too many points
  expect_error(
    sobol_design(lower = c(a = 0), upper = c(a = 1), nseq = 2^30 + 1),
    "too many points"
  )

  # Empty vectors
  expect_error(
    sobol_design(lower = numeric(0), upper = numeric(0), nseq = 10),
    "must not be empty"
  )

  # Lower >= upper
  expect_error(
    sobol_design(lower = c(a = 1), upper = c(a = 0), nseq = 10),
    "lower.*must be less than"
  )

  # Non-finite values
  expect_error(
    sobol_design(lower = c(a = 0), upper = c(a = Inf), nseq = 10),
    "finite values"
  )
})

test_that("sobol_design works with high dimensions", {
  params <- paste0("p", 1:10)
  design <- sobol_design(
    lower = setNames(rep(0, 10), params),
    upper = setNames(rep(1, 10), params),
    nseq = 100
  )

  expect_equal(nrow(design), 100)
  expect_equal(ncol(design), 10)
  expect_equal(names(design), params)
  expect_true(all(design >= 0 & design <= 1))
})

test_that("sobol_design is reproducible", {
  design1 <- sobol_design(
    lower = c(a = 0, b = 0),
    upper = c(a = 1, b = 1),
    nseq = 100
  )

  design2 <- sobol_design(
    lower = c(a = 0, b = 0),
    upper = c(a = 1, b = 1),
    nseq = 100
  )

  expect_equal(design1, design2)
})

test_that("sobol_design produces valid Sobol sequence properties", {
  # Generate design
  design <- sobol_design(
    lower = c(x = 0, y = 0),
    upper = c(x = 1, y = 1),
    nseq = 1000
  )

  # First point should be close to [0, 0] (actually exactly [0, 0])
  expect_true(design$x[1] == 0 && design$y[1] == 0)

  # Second point should be close to [0.5, 0.5]
  expect_true(abs(design$x[2] - 0.5) < 0.01)
  expect_true(abs(design$y[2] - 0.5) < 0.01)

  # Coverage test: all quadrants should be represented
  q1 <- sum(design$x < 0.5 & design$y < 0.5)
  q2 <- sum(design$x >= 0.5 & design$y < 0.5)
  q3 <- sum(design$x < 0.5 & design$y >= 0.5)
  q4 <- sum(design$x >= 0.5 & design$y >= 0.5)

  # Each quadrant should have roughly 25% of points
  expect_true(q1 > 200 && q1 < 300)
  expect_true(q2 > 200 && q2 < 300)
  expect_true(q3 > 200 && q3 < 300)
  expect_true(q4 > 200 && q4 < 300)
})

test_that("sobol_design API matches pomp-explore expectations", {
  # This test validates that the API is compatible with pomp-explore's sobol_design

  # Example from pomp-explore tests
  design <- sobol_design(
    lower = c(a = 0, b = 100),
    upper = c(b = 200, a = 1),
    nseq = 100
  )

  # Should return data frame
  expect_s3_class(design, "data.frame")

  # Should have correct dimensions
  expect_equal(nrow(design), 100)
  expect_equal(ncol(design), 2)

  # Should preserve parameter names from lower
  expect_equal(names(design), c("a", "b"))

  # Should scale correctly
  expect_true(all(design$a >= 0 & design$a <= 1))
  expect_true(all(design$b >= 100 & design$b <= 200))
})
