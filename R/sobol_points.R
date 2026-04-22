#' Generate a Batch of Sobol Points
#'
#' Efficiently generates a matrix of Sobol sequence points. This is a
#' convenience function that does not maintain state between calls.
#' For incremental generation, use \code{\link{sobol_generator}} instead.
#'
#' @param n Integer, the number of points to generate. Must be non-negative.
#' @param dimensions Integer, the number of dimensions for each point.
#'   Must be a positive integer.
#' @param skip Numeric, the number of initial points to skip (default: 0).
#'   This allows generating subsequences of the Sobol sequence.
#'
#' @return A numeric matrix with n rows and dimensions columns, where each
#'   row represents a point in the Sobol sequence. Values are in [0, 1).
#'   If n = 0, returns a 0 x dimensions matrix.
#'
#' @details
#' This function is implemented directly in C++ via Rcpp and is more
#' efficient than creating a generator and calling \code{sobol_next_n}
#' when you know in advance how many points you need.
#'
#' The skip parameter allows you to start from any point in the sequence,
#' which is useful for:
#' \itemize{
#'   \item Reproducibility: generating the same subsequence across runs
#'   \item Parallelization: different workers can generate non-overlapping segments
#'   \item Continuation: extending a previous sequence without regenerating early points
#' }
#'
#' @examples
#' \dontrun{
#' # Generate 1000 points in 5 dimensions
#' points <- sobol_points(n = 1000, dimensions = 5)
#' dim(points)  # [1] 1000    5
#'
#' # Skip the first 100 points
#' points_skipped <- sobol_points(n = 100, dimensions = 2, skip = 100)
#'
#' # Empty result
#' empty <- sobol_points(n = 0, dimensions = 3)
#' dim(empty)  # [1] 0 3
#' }
#'
#' @seealso \code{\link{sobol_generator}} for incremental generation with state
#'
#' @export
#' @useDynLib sobol, .registration = TRUE
#' @importFrom Rcpp sourceCpp
NULL

# Note: The actual sobol_points function is exported from C++ via Rcpp::export
# in src/rcpp_interface.cpp. This documentation file ensures it appears in
# the R documentation with proper help text.
