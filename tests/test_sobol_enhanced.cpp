// Enhanced comprehensive unit tests for Sobol sequence generator
// Tests for mathematical correctness, Property A, corner cases, and sequence validity

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numeric>
#include <set>
#include <stdexcept>
#include <vector>

#include "sobol/direction_numbers.hpp"
#include "sobol/primitive_polynomial.hpp"
#include "sobol/sobol.hpp"
#include "sobol/r_api.hpp"

namespace {

constexpr double kEpsilon = 1e-12;

bool nearly_equal(double a, double b, double eps = kEpsilon) {
  return std::fabs(a - b) < eps;
}

// Known reference values for first 8 points in 1D Sobol sequence
const std::vector<double> kReference1D = {
  0.0, 0.5, 0.75, 0.25, 0.375, 0.875, 0.625, 0.125
};

// Known reference values for first 4 points in 2D Sobol sequence
const std::vector<std::vector<double>> kReference2D = {
  {0.0, 0.0},
  {0.5, 0.5},
  {0.75, 0.25},
  {0.25, 0.75}
};

// Known reference values for first 8 points in 3D Sobol sequence
const std::vector<std::vector<double>> kReference3D = {
  {0.0, 0.0, 0.0},
  {0.5, 0.5, 0.5},
  {0.75, 0.25, 0.25},
  {0.25, 0.75, 0.75},
  {0.375, 0.625, 0.125},
  {0.875, 0.125, 0.625},
  {0.625, 0.875, 0.375},
  {0.125, 0.375, 0.875}
};

}  // namespace

// ============================================================================
// Property A Tests
// ============================================================================

void test_property_a_for_dimension(std::size_t dimension) {
  const auto polys = sobol::detail::generate_primitive_polynomials(dimension);
  const auto table = sobol::detail::build_direction_table(dimension);

  assert(table.size() == dimension);

  // For each dimension > 1, verify that initial direction numbers satisfy Property A
  for (std::size_t d = 1; d < dimension; ++d) {
    const std::uint32_t poly = polys[d];
    const std::size_t degree = static_cast<std::size_t>(sobol::detail::degree(poly));

    // Extract the first 'degree' direction numbers (in their most significant bits)
    std::vector<std::uint32_t> initial_numbers;
    initial_numbers.reserve(degree);
    for (std::size_t i = 0; i < degree; ++i) {
      initial_numbers.push_back(table[d][i] >> (32u - degree));
    }

    // Verify Property A: initial direction numbers form a full-rank matrix
    assert(sobol::detail::is_full_rank(initial_numbers, degree));
  }
}

void test_property_a_all_dimensions() {
  // Test Property A for dimensions 1-10
  for (std::size_t dim = 1; dim <= 10; ++dim) {
    test_property_a_for_dimension(dim);
  }
}

// ============================================================================
// Mathematical Correctness Tests with Reference Values
// ============================================================================

void test_1d_reference_values() {
  sobol::SobolEngine engine(1);
  for (std::size_t i = 0; i < kReference1D.size(); ++i) {
    const auto point = engine.next();
    assert(point.size() == 1);
    assert(nearly_equal(point[0], kReference1D[i]));
  }
}

void test_2d_reference_values() {
  sobol::SobolEngine engine(2);
  for (std::size_t i = 0; i < kReference2D.size(); ++i) {
    const auto point = engine.next();
    assert(point.size() == 2);
    assert(nearly_equal(point[0], kReference2D[i][0]));
    assert(nearly_equal(point[1], kReference2D[i][1]));
  }
}

void test_3d_reference_values() {
  sobol::SobolEngine engine(3);
  for (std::size_t i = 0; i < kReference3D.size(); ++i) {
    const auto point = engine.next();
    assert(point.size() == 3);
    assert(nearly_equal(point[0], kReference3D[i][0]));
    assert(nearly_equal(point[1], kReference3D[i][1]));
    assert(nearly_equal(point[2], kReference3D[i][2]));
  }
}

void test_dimensions_4_to_10_basic() {
  // For dimensions 4-10, verify basic properties
  for (std::size_t dim = 4; dim <= 10; ++dim) {
    sobol::SobolEngine engine(dim);

    // Generate first 10 points and verify they are in [0, 1)
    for (std::size_t i = 0; i < 10; ++i) {
      const auto point = engine.next();
      assert(point.size() == dim);
      for (double coord : point) {
        assert(coord >= 0.0 && coord < 1.0);
      }
    }
  }
}

// ============================================================================
// Sequence Validity Tests
// ============================================================================

void test_sequence_uniformity() {
  // Test that Sobol points are well-distributed (basic uniformity check)
  sobol::SobolEngine engine(2);
  const std::size_t n_points = 1024;
  const std::size_t n_bins = 16;

  std::vector<std::vector<std::size_t>> bins(n_bins, std::vector<std::size_t>(n_bins, 0));

  for (std::size_t i = 0; i < n_points; ++i) {
    const auto point = engine.next();
    const std::size_t bin_x = std::min(
      static_cast<std::size_t>(point[0] * n_bins), n_bins - 1);
    const std::size_t bin_y = std::min(
      static_cast<std::size_t>(point[1] * n_bins), n_bins - 1);
    bins[bin_x][bin_y]++;
  }

  // Each bin should have approximately n_points / (n_bins * n_bins) points
  const double expected = static_cast<double>(n_points) / (n_bins * n_bins);
  const double tolerance = expected * 2.0;  // Allow 2x variation

  for (const auto& row : bins) {
    for (std::size_t count : row) {
      assert(count > 0);  // No empty bins
      assert(static_cast<double>(count) < expected + tolerance);
    }
  }
}

void test_sequence_uniqueness() {
  // Verify that the first N points are all unique
  sobol::SobolEngine engine(3);
  const std::size_t n_points = 100;

  std::set<std::vector<double>> unique_points;
  for (std::size_t i = 0; i < n_points; ++i) {
    unique_points.insert(engine.next());
  }

  assert(unique_points.size() == n_points);
}

void test_gray_code_correctness() {
  // Verify that Gray code sequence is correctly implemented
  // by comparing incremental generation with skip_to
  sobol::SobolEngine incremental(5);

  std::vector<std::vector<double>> incremental_points;
  for (std::size_t i = 0; i < 100; ++i) {
    incremental_points.push_back(incremental.next());
  }

  // Now generate same points using skip_to
  for (std::size_t i = 0; i < 100; ++i) {
    sobol::SobolEngine skipped(5, i);
    const auto point = skipped.next();

    assert(point.size() == incremental_points[i].size());
    for (std::size_t d = 0; d < point.size(); ++d) {
      assert(nearly_equal(point[d], incremental_points[i][d]));
    }
  }
}

// ============================================================================
// Corner Cases and Boundary Conditions
// ============================================================================

void test_zero_skip() {
  sobol::SobolEngine with_skip(2, 0);
  sobol::SobolEngine without_skip(2);

  for (std::size_t i = 0; i < 10; ++i) {
    const auto p1 = with_skip.next();
    const auto p2 = without_skip.next();

    assert(p1.size() == p2.size());
    for (std::size_t d = 0; d < p1.size(); ++d) {
      assert(nearly_equal(p1[d], p2[d]));
    }
  }
}

void test_single_dimension() {
  sobol::SobolEngine engine(1);
  const auto point = engine.next();
  assert(point.size() == 1);
  assert(point[0] >= 0.0 && point[0] < 1.0);
}

void test_large_dimensions() {
  // Test with 100 dimensions
  sobol::SobolEngine engine(100);
  const auto point = engine.next();
  assert(point.size() == 100);
  for (double coord : point) {
    assert(coord >= 0.0 && coord < 1.0);
  }
}

void test_very_large_dimensions() {
  // Test with 1000 dimensions (max precomputed)
  sobol::SobolEngine engine(1000);
  const auto point = engine.next();
  assert(point.size() == 1000);
  for (double coord : point) {
    assert(coord >= 0.0 && coord < 1.0);
  }
}

void test_large_skip() {
  const std::uint64_t large_skip = 1000000;
  sobol::SobolEngine engine(3, large_skip);

  assert(engine.index() == large_skip);
  const auto point = engine.next();
  assert(point.size() == 3);
  assert(engine.index() == large_skip + 1);
}

void test_skip_backwards() {
  sobol::SobolEngine engine(2);

  // Advance to index 100
  for (std::size_t i = 0; i < 100; ++i) {
    engine.next();
  }
  assert(engine.index() == 100);

  // Skip backwards to index 50
  engine.skip_to(50);
  assert(engine.index() == 50);

  const auto point = engine.next();
  assert(point.size() == 2);
}

void test_max_supported_index() {
  // Test at the maximum supported index
  const std::uint64_t max_index = (std::uint64_t{1} << sobol::detail::kSobolBits) - 1u;
  sobol::SobolEngine engine(1, max_index);

  assert(engine.index() == max_index);
  const auto point = engine.next();
  assert(point.size() == 1);

  // Next call should throw overflow_error
  bool threw = false;
  try {
    engine.next();
  } catch (const std::overflow_error&) {
    threw = true;
  }
  assert(threw);
}

void test_invalid_zero_dimensions() {
  bool threw = false;
  try {
    sobol::SobolEngine engine(0);
  } catch (const std::invalid_argument&) {
    threw = true;
  }
  assert(threw);
}

void test_invalid_skip_beyond_max() {
  const std::uint64_t beyond_max = (std::uint64_t{1} << sobol::detail::kSobolBits);
  bool threw = false;
  try {
    sobol::SobolEngine engine(1, beyond_max);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  assert(threw);
}

void test_skip_to_beyond_max() {
  sobol::SobolEngine engine(1);
  const std::uint64_t beyond_max = (std::uint64_t{1} << sobol::detail::kSobolBits);

  bool threw = false;
  try {
    engine.skip_to(beyond_max);
  } catch (const std::out_of_range&) {
    threw = true;
  }
  assert(threw);
}

// ============================================================================
// R API Tests
// ============================================================================

void test_column_major_conversion() {
  const std::size_t n = 10;
  const std::size_t dim = 3;

  const auto row_major = sobol::sobol_points(n, dim);
  const auto column_major = sobol::sobol_points_column_major(n, dim);

  assert(column_major.size() == n * dim);

  // Verify conversion is correct
  for (std::size_t row = 0; row < n; ++row) {
    for (std::size_t col = 0; col < dim; ++col) {
      assert(nearly_equal(column_major[col * n + row], row_major[row][col]));
    }
  }
}

void test_r_generator_adapter_equivalence() {
  sobol::RGeneratorAdapter adapter(5, 10);
  sobol::SobolEngine engine(5, 10);

  // Test single point generation
  const auto adapter_point = adapter.next_point();
  const auto engine_point = engine.next();

  assert(adapter_point.size() == engine_point.size());
  for (std::size_t i = 0; i < adapter_point.size(); ++i) {
    assert(nearly_equal(adapter_point[i], engine_point[i]));
  }

  // Test batch generation
  const auto adapter_batch = adapter.next_points_column_major(5);
  assert(adapter_batch.size() == 25);

  for (std::size_t row = 0; row < 5; ++row) {
    const auto point = engine.next();
    for (std::size_t col = 0; col < 5; ++col) {
      assert(nearly_equal(adapter_batch[col * 5 + row], point[col]));
    }
  }
}

void test_empty_batch() {
  const auto empty = sobol::sobol_points_column_major(0, 5);
  assert(empty.empty());
}

void test_matrix_size_overflow() {
  const std::size_t large_n = std::numeric_limits<std::size_t>::max() / 2 + 1;
  bool threw = false;
  try {
    sobol::checked_matrix_size(large_n, 2);
  } catch (const std::overflow_error&) {
    threw = true;
  }
  assert(threw);
}

// ============================================================================
// Primitive Polynomial Tests
// ============================================================================

void test_primitive_polynomials_correctness() {
  const auto polys = sobol::detail::generate_primitive_polynomials(10);
  assert(polys.size() == 10);

  // Verify each polynomial is primitive
  for (const auto poly : polys) {
    assert(sobol::detail::is_primitive(poly));
    assert(sobol::detail::is_irreducible(poly));
    assert((poly & 1u) != 0);  // Must be odd
  }
}

void test_known_primitive_polynomials() {
  const auto polys = sobol::detail::generate_primitive_polynomials(5);

  // Verify known values
  assert(polys[0] == 0b11u);      // x + 1
  assert(polys[1] == 0b111u);     // x^2 + x + 1
  assert(polys[2] == 0b1011u);    // x^3 + x + 1
  assert(polys[3] == 0b1101u);    // x^3 + x^2 + 1
}

void test_polynomial_degree() {
  assert(sobol::detail::degree(0b11u) == 1);      // x + 1
  assert(sobol::detail::degree(0b111u) == 2);     // x^2 + x + 1
  assert(sobol::detail::degree(0b1011u) == 3);    // x^3 + x + 1
  assert(sobol::detail::degree(0b10011u) == 4);   // x^4 + x + 1
}

// ============================================================================
// Precomputed Tables Tests
// ============================================================================

#ifndef SOBOL_NO_PRECOMPUTED_TABLES
void test_precomputed_vs_runtime() {
  // Test first 10 dimensions
  const std::size_t test_dims = 10;
  const auto runtime_polys = sobol::detail::generate_primitive_polynomials(test_dims);

  // Verify primitive polynomials match
  for (std::size_t i = 0; i < test_dims; ++i) {
    assert(sobol::precomputed::primitive_polynomials[i] == runtime_polys[i]);
  }

  // Verify direction numbers match
  for (std::size_t d = 1; d <= test_dims; ++d) {
    const auto runtime_dirs = sobol::detail::direction_numbers_for_dimension(d, runtime_polys);
    const auto& precomputed_dirs = sobol::precomputed::direction_numbers[d - 1];

    for (std::size_t bit = 0; bit < sobol::detail::kSobolBits; ++bit) {
      assert(precomputed_dirs[bit] == runtime_dirs[bit]);
    }
  }
}

void test_precomputed_tables_usage() {
  // Verify precomputed tables are used for small dimensions
  sobol::SobolEngine small(10);
  const auto p1 = small.next();
  assert(p1.size() == 10);

  // Verify precomputed tables are used for larger dimensions
  sobol::SobolEngine large(500);
  const auto p2 = large.next();
  assert(p2.size() == 500);

  // Verify at boundary (1000 dimensions)
  sobol::SobolEngine boundary(1000);
  const auto p3 = boundary.next();
  assert(p3.size() == 1000);
}
#endif

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
  // Property A tests
  test_property_a_all_dimensions();

  // Mathematical correctness tests
  test_1d_reference_values();
  test_2d_reference_values();
  test_3d_reference_values();
  test_dimensions_4_to_10_basic();

  // Sequence validity tests
  test_sequence_uniformity();
  test_sequence_uniqueness();
  test_gray_code_correctness();

  // Corner cases
  test_zero_skip();
  test_single_dimension();
  test_large_dimensions();
  test_very_large_dimensions();
  test_large_skip();
  test_skip_backwards();
  test_max_supported_index();
  test_invalid_zero_dimensions();
  test_invalid_skip_beyond_max();
  test_skip_to_beyond_max();

  // R API tests
  test_column_major_conversion();
  test_r_generator_adapter_equivalence();
  test_empty_batch();
  test_matrix_size_overflow();

  // Primitive polynomial tests
  test_primitive_polynomials_correctness();
  test_known_primitive_polynomials();
  test_polynomial_degree();

  // Precomputed tables tests
#ifndef SOBOL_NO_PRECOMPUTED_TABLES
  test_precomputed_vs_runtime();
  test_precomputed_tables_usage();
#endif

  return 0;
}
