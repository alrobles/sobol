#' @keywords internal
"_PACKAGE"

# The following block is used by usethis to automatically manage
# roxygen namespace tags. Modify with care!
## usethis namespace: start
#' @useDynLib sobol, .registration = TRUE
#' @importFrom Rcpp sourceCpp
## usethis namespace: end
NULL

# Load the Rcpp module when the package is loaded
.onLoad <- function(libname, pkgname) {
  # The module will be automatically loaded by Rcpp
  # This ensures the SobolGenerator class is available
}
