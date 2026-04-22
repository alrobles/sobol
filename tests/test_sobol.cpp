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

  return 0;
}
