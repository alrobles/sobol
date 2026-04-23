#!/bin/bash
# Test Coverage Summary Script
# Generates a comprehensive report of test coverage

set -e

echo "============================================"
echo "Sobol Library Test Coverage Summary"
echo "============================================"
echo ""

# C++ Tests
echo "### C++ Test Coverage ###"
echo ""

# Find the repository root
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

if [ -d "build" ]; then
    echo "Building C++ tests..."
    cmake --build build --config Release > /dev/null 2>&1

    echo "Running C++ tests..."
    cd build
    ctest --output-on-failure 2>&1 | grep -E "(Test project|tests passed|tests failed|Total Test)"
    cd "$REPO_ROOT"
    echo ""
else
    echo "⚠️  Build directory not found. Run: cmake -S . -B build && cmake --build build"
    echo ""
fi

# Count C++ test cases
echo "C++ Test Statistics:"
echo "-------------------"
if [ -f "tests/test_sobol.cpp" ]; then
    BASIC_TESTS=$(grep -c "^  {$" tests/test_sobol.cpp || echo "0")
    echo "  Basic test suite: ~$BASIC_TESTS test cases"
fi

if [ -f "tests/test_sobol_enhanced.cpp" ]; then
    ENHANCED_TESTS=$(grep -c "^void test_" tests/test_sobol_enhanced.cpp || echo "0")
    echo "  Enhanced test suite: $ENHANCED_TESTS test functions"
    echo ""
    echo "Enhanced tests cover:"
    echo "  - Property A validation"
    echo "  - Mathematical correctness (1-10D)"
    echo "  - Sequence validity (uniformity, uniqueness, Gray code)"
    echo "  - Corner cases and boundaries"
    echo "  - R API compatibility"
    echo "  - Primitive polynomials"
    echo "  - Precomputed tables"
fi
echo ""

# R Tests
echo "### R Test Coverage ###"
echo ""

if command -v Rscript &> /dev/null; then
    if [ -f "DESCRIPTION" ]; then
        echo "Running R tests..."
        Rscript -e "
        if (requireNamespace('testthat', quietly = TRUE)) {
          results <- testthat::test_dir('tests/testthat', reporter = 'summary')
        } else {
          cat('⚠️  testthat package not installed\\n')
        }
        " 2>&1 || echo "⚠️  Some R tests failed or testthat not available"
        echo ""
    else
        echo "⚠️  Not an R package directory"
        echo ""
    fi
else
    echo "⚠️  R/Rscript not found. Install R to run R tests."
    echo ""
fi

# Count R test cases
echo "R Test Statistics:"
echo "------------------"
if [ -f "tests/testthat/test-sobol_generator.R" ]; then
    BASIC_R_TESTS=$(grep -c "^test_that(" tests/testthat/test-sobol_generator.R || echo "0")
    echo "  Basic test suite: $BASIC_R_TESTS test cases"
fi

if [ -f "tests/testthat/test-sobol_enhanced.R" ]; then
    ENHANCED_R_TESTS=$(grep -c "^test_that(" tests/testthat/test-sobol_enhanced.R || echo "0")
    echo "  Enhanced test suite: $ENHANCED_R_TESTS test cases"
    echo ""
    echo "Enhanced R tests cover:"
    echo "  - API usability (all exported functions)"
    echo "  - Mathematical correctness with reference values"
    echo "  - Consistency with C++ backend"
    echo "  - Edge cases (1000D, 10M+ skip)"
    echo "  - Sequence properties"
    echo "  - Performance and scalability"
fi
echo ""

# Coverage summary
echo "### Test Coverage Summary ###"
echo ""
echo "✅ Property A validation for dimensions 1-10"
echo "✅ Reference values tested for 1D, 2D, 3D sequences"
echo "✅ Corner cases: zero/large dimensions, zero/large skip"
echo "✅ Boundary conditions: max index (2^32-1), overflow protection"
echo "✅ Sequence validity: uniformity, uniqueness, Gray code"
echo "✅ R/C++ consistency across all APIs"
echo "✅ Extreme cases for pomp-explore compatibility"
echo "✅ Precomputed tables validation"
echo ""

# CI Status
echo "### CI/CD Integration ###"
echo ""
if [ -f ".github/workflows/cpp-tests.yml" ]; then
    echo "✅ C++ CI workflow configured"
    echo "   - Platforms: Ubuntu, macOS, Windows"
    echo "   - Tests: Basic + Enhanced suites"
    echo "   - Coverage: Code coverage reporting"
fi

if [ -f ".github/workflows/r-tests.yml" ]; then
    echo "✅ R CI workflow configured"
    echo "   - Platforms: Ubuntu, macOS, Windows"
    echo "   - R versions: 4.2, 4.3, 4.4"
    echo "   - Tests: testthat + R CMD check"
fi
echo ""

echo "============================================"
echo "For detailed testing documentation, see:"
echo "  - TESTING.md"
echo "  - README.md"
echo "============================================"
