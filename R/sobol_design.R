#' Generate a Sobol Design for Parameter Exploration
#'
#' Creates a Latin hypercube design based on the Sobol low-discrepancy sequence.
#' This function provides an API-compatible alternative to the sobol_design
#' function in the pomp-explore package for generating parameter designs.
#'
#' @param lower Named numeric vector giving the lower bounds of the parameter ranges.
#'   Must have the same names as \code{upper}.
#' @param upper Named numeric vector giving the upper bounds of the parameter ranges.
#'   Must have the same names as \code{lower}.
#' @param nseq Integer, the total number of parameter sets (points) to generate.
#'
#' @return A data frame with \code{nseq} rows and one column for each parameter
#'   named in \code{lower} and \code{upper}. Each column contains values scaled
#'   to the specified range [lower, upper] for that parameter.
#'
#' @details
#' This function generates a Sobol sequence in the unit hypercube [0,1]^d and
#' then scales each dimension to the specified parameter ranges. The Sobol
#' sequence is generated using the Joe-Kuo direction numbers with Property A
#' enforcement, providing excellent low-discrepancy properties.
#'
#' The function is designed to be API-compatible with the \code{sobol_design}
#' function from the pomp-explore package, allowing for easy comparison and
#' drop-in replacement.
#'
#' @examples
#' \dontrun{
#' # Generate 100 parameter sets for two parameters
#' design <- sobol_design(
#'   lower = c(a = 0, b = 100),
#'   upper = c(a = 1, b = 200),
#'   nseq = 100
#' )
#' head(design)
#'
#' # Plot the design
#' plot(design$a, design$b, main = "Sobol Design")
#'
#' # High-dimensional example
#' params <- paste0("param", 1:10)
#' design_10d <- sobol_design(
#'   lower = setNames(rep(0, 10), params),
#'   upper = setNames(rep(1, 10), params),
#'   nseq = 1000
#' )
#' }
#'
#' @references
#' Bratley, P., & Fox, B. L. (1988). Algorithm 659: Implementing Sobol's
#' quasirandom sequence generator. ACM Transactions on Mathematical Software,
#' 14(1), 88-100.
#'
#' Joe, S., & Kuo, F. Y. (2008). Constructing Sobol sequences with better
#' two-dimensional projections. SIAM Journal on Scientific Computing, 30(5),
#' 2635-2654.
#'
#' @seealso \code{\link{sobol_points}} for batch generation without scaling,
#'   \code{\link{sobol_generator}} for incremental generation
#'
#' @export
sobol_design <- function(lower = numeric(0), upper = numeric(0), nseq) {
  # Validate inputs
  if (length(lower) != length(upper)) {
    stop("'lower' and 'upper' must have same length")
  }

  if (length(lower) == 0) {
    stop("'lower' and 'upper' must not be empty")
  }

  lnames <- names(lower)
  if (is.null(lnames)) {
    stop("'lower' and 'upper' must be named vectors")
  }

  if (is.null(names(upper))) {
    stop("'lower' and 'upper' must be named vectors")
  }

  if (!all(sort(lnames) == sort(names(upper)))) {
    stop("names of 'lower' and 'upper' must match")
  }

  if (!is.numeric(nseq) || length(nseq) != 1 || !is.finite(nseq) ||
      nseq <= 0 || nseq != floor(nseq)) {
    stop("'nseq' must be a positive integer")
  }

  if (nseq > 1073741824L) {
    stop("too many points requested (maximum is 2^30)")
  }

  if (!all(is.finite(lower)) || !all(is.finite(upper))) {
    stop("'lower' and 'upper' must contain finite values")
  }

  # Reorder upper to match lower's names before validation
  upper <- upper[lnames]

  if (any(lower >= upper)) {
    stop("all elements of 'lower' must be less than corresponding elements of 'upper'")
  }

  # Get number of dimensions
  d <- length(lower)

  # Generate Sobol points in [0, 1]^d
  points <- tryCatch(
    sobol_points(n = as.integer(nseq), dim = as.integer(d), skip = 0),
    error = function(e) {
      stop("Failed to generate Sobol sequence: ", e$message)
    }
  )

  # Scale each dimension to [lower, upper]
  scaled <- vapply(
    seq_len(d),
    function(k) {
      lower[k] + (upper[k] - lower[k]) * points[, k]
    },
    numeric(nseq)
  )

  # Convert to data frame with proper column names
  colnames(scaled) <- lnames
  as.data.frame(scaled)
}
