// BSD 3-Clause License
//
// Copyright (c) 2025, Angel Robles
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
