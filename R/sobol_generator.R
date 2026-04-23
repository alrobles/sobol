#' Create a Sobol Sequence Generator
#'
#' Creates an S3 object that wraps an Rcpp Sobol sequence generator for
#' incremental point generation. The generator maintains internal state
#' and allows for efficient generation of quasi-random sequences.
#'
#' @param dimensions Integer, the number of dimensions for the Sobol sequence.
#'   Must be a positive integer.
#' @param skip Numeric, the number of initial points to skip (default: 0).
#'   This allows starting the sequence from any index for reproducibility.
#'
#' @return An S3 object of class "sobol_generator" containing:
#'   \item{generator}{The underlying Rcpp reference class object}
#'   \item{dimensions}{Number of dimensions}
#'   \item{initial_skip}{Initial skip value used at construction}
#'
#' @examples
#' \dontrun{
#' # Create a 3-dimensional Sobol generator
#' gen <- sobol_generator(dimensions = 3)
#'
#' # Generate a single point
#' point <- sobol_next(gen)
#'
#' # Generate multiple points
#' points <- sobol_next_n(gen, n = 100)
#'
#' # Skip to a specific index
#' sobol_skip_to(gen, 1000)
#'
#' # Create a generator starting from index 50
#' gen2 <- sobol_generator(dimensions = 2, skip = 50)
#' }
#'
#' @export
#' @importFrom methods new
sobol_generator <- function(dimensions, skip = 0) {
  # Validate inputs
  if (!is.numeric(dimensions) || length(dimensions) != 1 ||
        !is.finite(dimensions) || dimensions <= 0 ||
        dimensions != floor(dimensions)) {
    stop("'dimensions' must be a positive integer")
  }

  if (!is.numeric(skip) || length(skip) != 1 ||
        !is.finite(skip) || skip < 0 || skip != floor(skip)) {
    stop("'skip' must be a non-negative integer")
  }

  # Convert to integer types for R
  dimensions <- as.integer(dimensions)
  skip <- as.numeric(skip)

  # Load the Rcpp module
  sobol_module <- Rcpp::Module("sobol_generator_module", PACKAGE = "sobol")

  # Create the generator
  generator <- tryCatch(
    new(sobol_module$SobolGenerator, dimensions, skip),
    error = function(e) {
      stop("Failed to create Sobol generator: ", e$message)
    }
  )

  # Create S3 object
  obj <- structure(
    list(
      generator = generator,
      dimensions = dimensions,
      initial_skip = skip
    ),
    class = "sobol_generator"
  )

  obj
}

#' Generate the Next Point from a Sobol Generator
#'
#' Generates a single point from the Sobol sequence and advances the
#' internal state of the generator.
#'
#' @param x A sobol_generator object created by \code{\link{sobol_generator}}
#' @param ... Additional arguments (currently unused)
#'
#' @return A numeric vector of length equal to the number of dimensions,
#'   containing the next point in the Sobol sequence. Values are in [0, 1).
#'
#' @examples
#' \dontrun{
#' gen <- sobol_generator(dimensions = 3)
#' point <- sobol_next(gen)
#' print(point) # e.g., [0.5, 0.5, 0.5]
#' }
#'
#' @export
sobol_next <- function(x, ...) {
  if (!inherits(x, "sobol_generator")) {
    stop("'x' must be a sobol_generator object")
  }

  tryCatch(
    x$generator$`next`(),
    error = function(e) {
      stop("Failed to generate next point: ", e$message)
    }
  )
}

#' Generate Multiple Points from a Sobol Generator
#'
#' Generates n consecutive points from the Sobol sequence and advances the
#' internal state of the generator.
#'
#' @param x A sobol_generator object created by \code{\link{sobol_generator}}
#' @param n Integer, the number of points to generate. Must be non-negative.
#' @param ... Additional arguments (currently unused)
#'
#' @return A numeric matrix with n rows and dimensions columns, where each
#'   row represents a point in the Sobol sequence. Values are in [0, 1).
#'   If n = 0, returns a 0 x dimensions matrix.
#'
#' @examples
#' \dontrun{
#' gen <- sobol_generator(dimensions = 2)
#' points <- sobol_next_n(gen, n = 10)
#' print(dim(points)) # [1] 10  2
#' }
#'
#' @export
sobol_next_n <- function(x, n, ...) {
  if (!inherits(x, "sobol_generator")) {
    stop("'x' must be a sobol_generator object")
  }

  if (!is.numeric(n) || length(n) != 1 || !is.finite(n) || n < 0 ||
        n != floor(n)) {
    stop("'n' must be a non-negative integer")
  }

  n <- as.integer(n)

  tryCatch(
    x$generator$next_n(n),
    error = function(e) {
      stop("Failed to generate points: ", e$message)
    }
  )
}

#' Skip to a Specific Index in a Sobol Generator
#'
#' Jumps the internal state of the generator to a specific index in the
#' Sobol sequence. This allows for reproducible subsequences and parallel
#' generation strategies.
#'
#' @param x A sobol_generator object created by \code{\link{sobol_generator}}
#' @param index Numeric, the index to skip to. Must be a non-negative integer.
#'   The next call to \code{sobol_next} will return the point at this index.
#' @param ... Additional arguments (currently unused)
#'
#' @return Invisibly returns the sobol_generator object (for chaining).
#'   The primary purpose is the side effect of updating the internal state.
#'
#' @examples
#' \dontrun{
#' gen <- sobol_generator(dimensions = 2)
#' sobol_skip_to(gen, 100)
#' point <- sobol_next(gen) # This is the 100th point (0-indexed)
#' }
#'
#' @export
sobol_skip_to <- function(x, index, ...) {
  if (!inherits(x, "sobol_generator")) {
    stop("'x' must be a sobol_generator object")
  }

  if (!is.numeric(index) || length(index) != 1 || !is.finite(index) || index < 0
      || index != floor(index)) {
    stop("'index' must be a non-negative integer")
  }

  index <- as.numeric(index)

  tryCatch(
    x$generator$skip_to(index),
    error = function(e) {
      stop("Failed to skip to index: ", e$message)
    }
  )

  # Return the object invisibly for chaining (CRAN compliance - not just NULL)
  invisible(x)
}

#' Get Current Index of a Sobol Generator
#'
#' Returns the current index of the generator, which is the index of the
#' next point that will be generated.
#'
#' @param x A sobol_generator object created by \code{\link{sobol_generator}}
#' @param ... Additional arguments (currently unused)
#'
#' @return A numeric value representing the current index (0-based).
#'
#' @examples
#' \dontrun{
#' gen <- sobol_generator(dimensions = 2)
#' sobol_next(gen)
#' sobol_next(gen)
#' idx <- sobol_index(gen)
#' print(idx) # 2
#' }
#'
#' @export
sobol_index <- function(x, ...) {
  if (!inherits(x, "sobol_generator")) {
    stop("'x' must be a sobol_generator object")
  }

  tryCatch(
    x$generator$index(),
    error = function(e) {
      stop("Failed to get index: ", e$message)
    }
  )
}

#' Get Number of Dimensions of a Sobol Generator
#'
#' Returns the number of dimensions configured for the generator.
#'
#' @param x A sobol_generator object created by \code{\link{sobol_generator}}
#' @param ... Additional arguments (currently unused)
#'
#' @return An integer representing the number of dimensions.
#'
#' @examples
#' \dontrun{
#' gen <- sobol_generator(dimensions = 5)
#' dims <- sobol_dimensions(gen)
#' print(dims) # 5
#' }
#'
#' @export
sobol_dimensions <- function(x, ...) {
  if (!inherits(x, "sobol_generator")) {
    stop("'x' must be a sobol_generator object")
  }

  tryCatch(
    x$generator$dimensions(),
    error = function(e) {
      stop("Failed to get dimensions: ", e$message)
    }
  )
}

#' Print Method for Sobol Generator
#'
#' @param x A sobol_generator object
#' @param ... Additional arguments passed to print
#'
#' @return Invisibly returns the input object (for chaining)
#'
#' @export
print.sobol_generator <- function(x, ...) {
  cat("<Sobol Sequence Generator>\n")
  cat(sprintf("  Dimensions: %d\n", x$dimensions))
  cat(sprintf("  Initial skip: %g\n", x$initial_skip))

  # Get current index
  current_idx <- tryCatch(
    x$generator$index(),
    error = function(e) NA
  )

  if (!is.na(current_idx)) {
    cat(sprintf("  Current index: %g\n", current_idx))
    cat(sprintf("  Points generated: %g\n", current_idx - x$initial_skip))
  }

  invisible(x)
}

#' Summary Method for Sobol Generator
#'
#' @param object A sobol_generator object
#' @param ... Additional arguments passed to summary
#'
#' @return A list with class "summary.sobol_generator" containing summary
#' information
#'
#' @export
summary.sobol_generator <- function(object, ...) {
  current_idx <- tryCatch(
    object$generator$index(),
    error = function(e) NA
  )

  info <- list(
    dimensions = object$dimensions,
    initial_skip = object$initial_skip,
    current_index = current_idx,
    points_generated = if (!is.na(current_idx))
      current_idx - object$initial_skip else NA
  )

  class(info) <- "summary.sobol_generator"
  return(info)
}

#' Print Summary of Sobol Generator
#'
#' @param x A summary.sobol_generator object
#' @param ... Additional arguments passed to print
#'
#' @return Invisibly returns the input object
#'
#' @export
print.summary.sobol_generator <- function(x, ...) {
  cat("Sobol Sequence Generator Summary\n")
  cat("================================\n")
  cat(sprintf("Dimensions:        %d\n", x$dimensions))
  cat(sprintf("Initial skip:      %g\n", x$initial_skip))

  if (!is.na(x$current_index)) {
    cat(sprintf("Current index:     %g\n", x$current_index))
    cat(sprintf("Points generated:  %g\n", x$points_generated))
  } else {
    cat("Current index:     <unavailable>\n")
  }

  invisible(x)
}
