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

#include <algorithm>
#include <Rcpp.h>

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

#include "sobol/r_api.hpp"

namespace {

std::size_t validate_count(int value, const char* name, bool zero_allowed) {
  if (value < 0) {
    throw std::invalid_argument(std::string(name) + " must be non-negative");
  }
  if (!zero_allowed && value == 0) {
    throw std::invalid_argument(std::string(name) + " must be greater than zero");
  }
  return static_cast<std::size_t>(value);
}

std::uint64_t validate_index(double value, const char* name) {
  if (!std::isfinite(value)) {
    throw std::invalid_argument(std::string(name) + " must be finite");
  }
  if (value < 0.0) {
    throw std::invalid_argument(std::string(name) + " must be non-negative");
  }
  if (std::floor(value) != value) {
    throw std::invalid_argument(std::string(name) + " must be an integer value");
  }
  if (value > static_cast<double>(std::numeric_limits<std::uint64_t>::max())) {
    throw std::out_of_range(std::string(name) + " exceeds uint64 range");
  }
  return static_cast<std::uint64_t>(value);
}

class SobolGenerator {
 public:
  SobolGenerator(int dimensions, double skip = 0.0)
      : adapter_(validate_count(dimensions, "dim", false), validate_index(skip, "skip")) {}

  Rcpp::NumericVector next() {
    try {
      const auto point = adapter_.next_point();
      return Rcpp::NumericVector(point.begin(), point.end());
    } catch (const std::exception& e) {
      Rcpp::stop(std::string("SobolGenerator::next failed: ") + e.what());
    }
  }

  Rcpp::NumericMatrix next_n(int n) {
    try {
      const std::size_t count = validate_count(n, "n", true);
      const std::size_t dimensions = adapter_.dimensions();
      Rcpp::NumericMatrix out(static_cast<int>(count), static_cast<int>(dimensions));
      const auto column_major = adapter_.next_points_column_major(count);
      std::copy(column_major.begin(), column_major.end(), out.begin());
      return out;
    } catch (const std::exception& e) {
      Rcpp::stop(std::string("SobolGenerator::next_n failed: ") + e.what());
    }
  }

  void skip_to(double point_index) {
    try {
      adapter_.skip_to(validate_index(point_index, "point_index"));
    } catch (const std::exception& e) {
      Rcpp::stop(std::string("SobolGenerator::skip_to failed: ") + e.what());
    }
  }

  double index() const { return static_cast<double>(adapter_.index()); }
  int dimensions() const { return static_cast<int>(adapter_.dimensions()); }

 private:
  sobol::RGeneratorAdapter adapter_;
};

}  // namespace

// Generate an n x dim Sobol matrix.
// [[Rcpp::export]]
Rcpp::NumericMatrix sobol_points(int n, int dim, double skip = 0.0) {
  try {
    const std::size_t count = validate_count(n, "n", true);
    const std::size_t dimensions = validate_count(dim, "dim", false);
    const std::uint64_t skip_value = validate_index(skip, "skip");

    Rcpp::NumericMatrix out(static_cast<int>(count), static_cast<int>(dimensions));
    const auto column_major = sobol::sobol_points_column_major(count, dimensions, skip_value);
    std::copy(column_major.begin(), column_major.end(), out.begin());
    return out;
  } catch (const std::exception& e) {
    Rcpp::stop(std::string("sobol_points failed: ") + e.what());
  }
}

// Rcpp module exposing an incremental Sobol generator:
// - next(): one point
// - next_n(n): n points as a matrix
// - skip_to(k): jump to index k
RCPP_MODULE(sobol_generator_module) {
  Rcpp::class_<SobolGenerator>("SobolGenerator")
      .constructor<int, double>()
      .method("next", &SobolGenerator::next)
      .method("next_n", &SobolGenerator::next_n)
      .method("skip_to", &SobolGenerator::skip_to)
      .method("index", &SobolGenerator::index)
      .method("dimensions", &SobolGenerator::dimensions);
}
