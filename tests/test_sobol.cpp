#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include "sobol/direction_numbers.hpp"
#include "sobol/primitive_polynomial.hpp"
#include "sobol/sobol.hpp"
#include "sobol/r_api.hpp"

namespace {

bool nearly_equal(double a, double b, double eps = 1e-12) {
  return std::fabs(a - b) < eps;
}

}  // namespace

int main() {
  {
    const auto polys = sobol::detail::generate_primitive_polynomials(5);
    assert(polys.size() == 5u);
    assert(polys[0] == 0b11u);
    assert(polys[1] == 0b111u);
    assert(polys[2] == 0b1011u);
    assert(polys[3] == 0b1101u);
  }

  {
    sobol::SobolEngine engine(1u);
    const std::vector<double> expected{0.0, 0.5, 0.75, 0.25, 0.375, 0.875, 0.625, 0.125};
    for (double value : expected) {
      const auto point = engine.next();
      assert(point.size() == 1u);
      assert(nearly_equal(point[0], value));
    }
  }

  {
    sobol::SobolEngine from_start(3u);
    for (int skip_count = 0; skip_count < 17; ++skip_count) {
      (void)from_start.next();
    }
    const auto expected = from_start.next();

    sobol::SobolEngine skipped(3u, 17u);
    const auto actual = skipped.next();

    assert(actual.size() == expected.size());
    for (std::size_t i = 0u; i < actual.size(); ++i) {
      assert(nearly_equal(actual[i], expected[i]));
    }
  }

  {
    const auto table = sobol::detail::build_direction_table(8u);
    assert(table.size() == 8u);
    for (std::size_t dim = 1u; dim < table.size(); ++dim) {
      std::vector<std::uint32_t> rows;
      const auto degree = static_cast<std::size_t>(sobol::detail::degree(
          sobol::detail::generate_primitive_polynomials(8u)[dim]));
      rows.reserve(degree);
      for (std::size_t i = 0u; i < degree; ++i) {
        rows.push_back(table[dim][i] >> (32u - degree));
      }
      assert(sobol::detail::is_full_rank(rows, degree));
    }
  }

  {
    const auto points = sobol::sobol_points(4u, 2u, 3u);
    const auto column_major = sobol::sobol_points_column_major(4u, 2u, 3u);
    assert(column_major.size() == 8u);
    for (std::size_t col = 0u; col < 2u; ++col) {
      for (std::size_t row = 0u; row < 4u; ++row) {
        assert(nearly_equal(column_major[col * 4u + row], points[row][col]));
      }
    }
  }

  {
    sobol::RGeneratorAdapter adapter(2u, 5u);
    sobol::SobolEngine engine(2u, 5u);
    const auto adapter_point = adapter.next_point();
    const auto engine_point = engine.next();
    assert(adapter_point.size() == engine_point.size());
    for (std::size_t i = 0u; i < adapter_point.size(); ++i) {
      assert(nearly_equal(adapter_point[i], engine_point[i]));
    }

    const auto adapter_batch = adapter.next_points_column_major(3u);
    assert(adapter_batch.size() == 6u);
    for (std::size_t row = 0u; row < 3u; ++row) {
      const auto point = engine.next();
      for (std::size_t col = 0u; col < 2u; ++col) {
        assert(nearly_equal(adapter_batch[col * 3u + row], point[col]));
      }
    }
  }

  {
    bool threw = false;
    try {
      (void)sobol::RGeneratorAdapter(0u);
    } catch (const std::invalid_argument&) {
      threw = true;
    }
    assert(threw);
  }

  {
    const auto empty = sobol::sobol_points_column_major(0u, 2u);
    assert(empty.empty());
  }

  {
    bool threw = false;
    try {
      (void)sobol::sobol_points_column_major(4u, 0u);
    } catch (const std::invalid_argument&) {
      threw = true;
    }
    assert(threw);
  }

  {
    bool threw = false;
    try {
      (void)sobol::checked_matrix_size((std::numeric_limits<std::size_t>::max() / 2u) + 1u, 2u);
    } catch (const std::overflow_error&) {
      threw = true;
    }
    assert(threw);
  }

  {
    sobol::SobolEngine engine(1u);
    bool threw = false;
    try {
      engine.skip_to(std::uint64_t{1} << sobol::detail::kSobolBits);
    } catch (const std::out_of_range&) {
      threw = true;
    }
    assert(threw);
  }

  {
    bool threw = false;
    try {
      (void)sobol::SobolEngine(1u, std::uint64_t{1} << sobol::detail::kSobolBits);
    } catch (const std::out_of_range&) {
      threw = true;
    }
    assert(threw);
  }

  {
    sobol::SobolEngine engine(1u, (std::uint64_t{1} << sobol::detail::kSobolBits) - 1u);
    const auto point = engine.next();
    assert(point.size() == 1u);
    bool threw = false;
    try {
      (void)engine.next();
    } catch (const std::overflow_error&) {
      threw = true;
    }
    assert(threw);
  }

#ifndef SOBOL_NO_PRECOMPUTED_TABLES
  {
    // Verify precomputed tables match runtime generation for first 10 dimensions
    const std::size_t test_dims = 10u;
    const auto runtime_polys = sobol::detail::generate_primitive_polynomials(test_dims);

    // Check primitive polynomials match
    for (std::size_t i = 0u; i < test_dims; ++i) {
      assert(sobol::precomputed::primitive_polynomials[i] == runtime_polys[i]);
    }

    // Check direction numbers match
    for (std::size_t d = 1u; d <= test_dims; ++d) {
      const auto runtime_dirs = sobol::detail::direction_numbers_for_dimension(d, runtime_polys);
      const auto& precomputed_dirs = sobol::precomputed::direction_numbers[d - 1u];

      for (std::size_t bit = 0u; bit < sobol::detail::kSobolBits; ++bit) {
        assert(precomputed_dirs[bit] == runtime_dirs[bit]);
      }
    }
  }

  {
    // Verify that precomputed tables are actually used for dimensions <= 1000
    sobol::SobolEngine small_engine(10u);
    sobol::SobolEngine large_engine(100u);

    // Both should work correctly
    const auto p1 = small_engine.next();
    assert(p1.size() == 10u);

    const auto p2 = large_engine.next();
    assert(p2.size() == 100u);
  }
#endif

  return 0;
}
