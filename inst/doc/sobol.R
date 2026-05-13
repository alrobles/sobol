## ----include = FALSE----------------------------------------------------------
knitr::opts_chunk$set(
  collapse = TRUE,
  comment = "#>"
)

## ----eval = FALSE-------------------------------------------------------------
# # From GitHub
# devtools::install_github("alrobles/sobol")

## -----------------------------------------------------------------------------
library(sobol)

## -----------------------------------------------------------------------------
design <- sobol_design(
  lower = c(learning_rate = 0.0001, momentum = 0.00, dropout = 0.0),
  upper = c(learning_rate = 0.1000, momentum = 0.99, dropout = 0.5),
  nseq   = 200
)

head(design)

## -----------------------------------------------------------------------------
summary(design)

## ----eval = FALSE-------------------------------------------------------------
# # Use the design directly inside your optimisation loop
# results <- purrr::pmap_dbl(design, ~ my_model(lr = ..1, mom = ..2, drop = ..3))

## ----eval = FALSE-------------------------------------------------------------
# # Not run, but you can compare visual uniformity
# plot(design$learning_rate, design$momentum, col = "steelblue",
#      main = "Sobol design (200 points)")

## -----------------------------------------------------------------------------
raw <- sobol_points(n = 512, dim = 4)
dim(raw)           # 512 rows, 4 columns
range(raw)         # values in [0, 1)

## -----------------------------------------------------------------------------
gen <- sobol_generator(dim = 3)

# Generate one point
sobol_next(gen)

# Generate a batch of 50
batch <- sobol_next_n(gen, n = 50)
dim(batch)  # 50 x 3

# What’s the current index?
sobol_index(gen)

## -----------------------------------------------------------------------------
sobol_skip_to(gen, 1000)
sobol_index(gen)

## -----------------------------------------------------------------------------
a <- sobol_design(lower = c(p = 0), upper = c(p = 1), nseq = 32)
b <- sobol_design(lower = c(p = 0), upper = c(p = 1), nseq = 32)
identical(a, b)   # TRUE

## -----------------------------------------------------------------------------
# Worker 1
w1 <- sobol_design(lower = c(lr = 0.0001, mom = 0, drop = 0),
                   upper = c(lr = 0.1,    mom = 0.99, drop = 0.5),
                   nseq = 1000)  # implicitly starts at 0

# Worker 2 (needs raw points + skip to 1000)
raw2 <- sobol_points(n = 1000, dim = 3, skip = 1000)
# Then scale raw2 manually, or use sobol_design in the future with a skip argument

## -----------------------------------------------------------------------------
gen <- sobol_generator(dim = 2)
first_10 <- sobol_next_n(gen, n = 10)

# Oops, need to re‑evaluate the first 10 with different parameters
sobol_skip_to(gen, 0)
replicated <- sobol_next_n(gen, n = 10)
identical(first_10, replicated)  # TRUE

## -----------------------------------------------------------------------------
# Clean up
rm(design, raw, gen, a, b, first_10, replicated)

