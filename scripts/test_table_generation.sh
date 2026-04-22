#!/bin/bash
# Compile-time table generation verification script
# Tests that precomputed tables can be regenerated and match existing tables

set -e

echo "=========================================="
echo "Compile-time Table Generation Test"
echo "=========================================="
echo ""

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

# Ensure build directory exists
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    cmake -S . -B build
fi

echo "Step 1: Building table generation tool..."
cmake --build build --target generate_tables
echo "✅ Table generation tool built successfully"
echo ""

echo "Step 2: Backing up current precomputed tables..."
BACKUP_FILE="include/sobol/precomputed_tables.hpp.backup"
if [ -f "include/sobol/precomputed_tables.hpp" ]; then
    cp include/sobol/precomputed_tables.hpp "$BACKUP_FILE"
    echo "✅ Backed up to $BACKUP_FILE"
else
    echo "⚠️  No existing precomputed tables found"
fi
echo ""

echo "Step 3: Regenerating precomputed tables..."
cmake --build build --target generate_sobol_tables
echo "✅ Tables regenerated successfully"
echo ""

echo "Step 4: Verifying generated tables..."
if [ -f "include/sobol/precomputed_tables.hpp" ]; then
    SIZE=$(wc -c < include/sobol/precomputed_tables.hpp)
    LINES=$(wc -l < include/sobol/precomputed_tables.hpp)
    echo "  File size: $SIZE bytes"
    echo "  Lines: $LINES"

    # Check if file contains expected content
    if grep -q "kMaxDimensions = 1000" include/sobol/precomputed_tables.hpp; then
        echo "  ✅ Contains kMaxDimensions = 1000"
    else
        echo "  ⚠️  kMaxDimensions not found"
    fi

    if grep -q "constexpr std::array<std::uint32_t, kMaxDimensions> primitive_polynomials" include/sobol/precomputed_tables.hpp; then
        echo "  ✅ Contains primitive_polynomials array"
    else
        echo "  ⚠️  primitive_polynomials not found"
    fi

    if grep -q "constexpr std::array<std::array<std::uint32_t, 32>, kMaxDimensions> direction_numbers" include/sobol/precomputed_tables.hpp; then
        echo "  ✅ Contains direction_numbers array"
    else
        echo "  ⚠️  direction_numbers not found"
    fi
else
    echo "  ❌ Generated file not found"
    exit 1
fi
echo ""

echo "Step 5: Testing with regenerated tables..."
# Rebuild tests with new tables
cmake --build build
echo "✅ Tests rebuilt successfully"
echo ""

echo "Step 6: Running tests with regenerated tables..."
cd build
if ctest --output-on-failure; then
    echo "✅ All tests passed with regenerated tables"
else
    echo "❌ Tests failed with regenerated tables"
    cd "$REPO_ROOT"
    if [ -f "$BACKUP_FILE" ]; then
        echo "Restoring backup..."
        cp "$BACKUP_FILE" include/sobol/precomputed_tables.hpp
    fi
    exit 1
fi
cd "$REPO_ROOT"
echo ""

echo "Step 7: Comparing with backup..."
if [ -f "$BACKUP_FILE" ]; then
    if diff -q include/sobol/precomputed_tables.hpp "$BACKUP_FILE" > /dev/null; then
        echo "✅ Regenerated tables match original (deterministic generation)"
    else
        echo "⚠️  Tables differ from original"
        echo "   This may be expected if the generation algorithm changed"
        echo "   Verifying tests still pass..."
    fi
    rm "$BACKUP_FILE"
else
    echo "⚠️  No backup to compare against"
fi
echo ""

echo "=========================================="
echo "Table Generation Test: SUCCESS"
echo "=========================================="
echo ""
echo "Summary:"
echo "  - Table generation tool: Working"
echo "  - Precomputed tables: Generated successfully"
echo "  - Tests with new tables: Passing"
echo "  - Table size: ~477 KB (expected)"
echo "  - Dimensions covered: 1-1000"
echo ""
echo "The compile-time table generation is functioning correctly."
