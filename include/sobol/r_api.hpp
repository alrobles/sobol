#ifndef SOBOL_R_API_HPP
#define SOBOL_R_API_HPP

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include "sobol/sobol.hpp"

namespace sobol {

inline std::size_t checked_matrix_size(std::size_t n, std::size_t dimensions) {
  if (dimensions != 0u && n > std::numeric_limits<std::size_t>::max() / dimensions) {
    throw std::overflow_error("requested matrix size overflows size_t");
  }
  return n * dimensions;
}

// Utility adapter for Rcpp wrappers.
// `sobol_points_column_major` returns data in R matrix layout (column-major order).
inline std::vector<double> sobol_points_column_major(std::size_t n, std::size_t dimensions,
                                                     std::uint64_t skip = 0u) {
  if (dimensions == 0u) {
    throw std::invalid_argument("dimensions must be greater than zero");
  }
  std::vector<double> out(checked_matrix_size(n, dimensions));
  SobolEngine engine(dimensions, skip);

  for (std::size_t row = 0u; row < n; ++row) {
    const auto point = engine.next();
    for (std::size_t col = 0u; col < dimensions; ++col) {
      out[col * n + row] = point[col];
    }
  }
  return out;
}

class RGeneratorAdapter {
 public:
  explicit RGeneratorAdapter(std::size_t dimensions, std::uint64_t skip = 0u)
      : engine_(dimensions, skip) {}

  std::size_t dimensions() const noexcept { return engine_.dimensions(); }
  std::uint64_t index() const noexcept { return engine_.index(); }

  std::vector<double> next_point() { return engine_.next(); }

  std::vector<double> next_points_column_major(std::size_t n) {
    std::vector<double> out(checked_matrix_size(n, engine_.dimensions()));
    for (std::size_t row = 0u; row < n; ++row) {
      const auto point = engine_.next();
      for (std::size_t col = 0u; col < engine_.dimensions(); ++col) {
        out[col * n + row] = point[col];
      }
    }
    return out;
  }

  void skip_to(std::uint64_t point_index) { engine_.skip_to(point_index); }

 private:
  SobolEngine engine_;
};

}  // namespace sobol

#endif  // SOBOL_R_API_HPP
