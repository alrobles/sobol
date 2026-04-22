# S3 Implementation Summary for Sobol Package

## Overview

This document describes the S3 object-oriented system implementation for the Sobol sequence generator R package. The implementation wraps the existing Rcpp module to provide an idiomatic R interface.

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    R User Interface (S3)                     │
├─────────────────────────────────────────────────────────────┤
│  sobol_generator()        - S3 Constructor                   │
│  sobol_next()            - Generate single point             │
│  sobol_next_n()          - Generate multiple points          │
│  sobol_skip_to()         - Jump to index                     │
│  sobol_index()           - Query current index               │
│  sobol_dimensions()      - Query dimensions                  │
│  print.sobol_generator() - S3 print method                   │
│  summary.sobol_generator() - S3 summary method               │
├─────────────────────────────────────────────────────────────┤
│                    Rcpp Module Layer                         │
├─────────────────────────────────────────────────────────────┤
│  SobolGenerator (Rcpp Module)                                │
│    - next()                                                  │
│    - next_n(n)                                               │
│    - skip_to(index)                                          │
│    - index()                                                 │
│    - dimensions()                                            │
├─────────────────────────────────────────────────────────────┤
│                    C++ Core Library                          │
├─────────────────────────────────────────────────────────────┤
│  sobol::RGeneratorAdapter                                    │
│  sobol::SobolEngine                                          │
│  sobol::sobol_points_column_major()                          │
└─────────────────────────────────────────────────────────────┘
```

## S3 Class Structure

### sobol_generator

The main S3 class that represents a Sobol sequence generator.

**Structure:**
```r
sobol_generator <- list(
  generator = <Rcpp Reference Class>,
  dimensions = <integer>,
  initial_skip = <numeric>
)
class(sobol_generator) <- "sobol_generator"
```

**Attributes:**
- `generator`: The underlying Rcpp module object that maintains state
- `dimensions`: Number of dimensions (cached for quick access)
- `initial_skip`: The skip value used at construction (for reproducibility tracking)

## Constructor Pattern

Following R best practices, the constructor:

1. Validates input parameters
2. Converts types appropriately (R integers/numerics)
3. Loads the Rcpp module dynamically
4. Creates the underlying Rcpp object
5. Wraps it in a list with metadata
6. Assigns the S3 class
7. Returns the complete object

```r
sobol_generator <- function(dimensions, skip = 0) {
  # 1. Validate
  if (!is.numeric(dimensions) || length(dimensions) != 1 ||
      dimensions <= 0 || dimensions != floor(dimensions)) {
    stop("'dimensions' must be a positive integer")
  }

  # 2. Convert types
  dimensions <- as.integer(dimensions)
  skip <- as.numeric(skip)

  # 3-4. Load module and create object
  sobol_module <- Rcpp::Module("sobol_generator_module", PACKAGE = "sobol")
  generator <- new(sobol_module$SobolGenerator, dimensions, skip)

  # 5-6. Create S3 object
  obj <- structure(
    list(
      generator = generator,
      dimensions = dimensions,
      initial_skip = skip
    ),
    class = "sobol_generator"
  )

  # 7. Return
  return(obj)
}
```

## Wrapper Functions

All wrapper functions follow a consistent pattern:

1. Validate that the input is a `sobol_generator` object
2. Validate additional parameters
3. Call the underlying Rcpp method
4. Handle errors appropriately
5. Return results (never just NULL for CRAN compliance)

### Side-Effect Functions

For functions whose primary purpose is side effects (like `sobol_skip_to`), the implementation:
- Returns the object invisibly for method chaining
- Complies with CRAN policies (functions should return something)

```r
sobol_skip_to <- function(x, index, ...) {
  # Validation...
  x$generator$skip_to(index)

  # Return object invisibly (CRAN compliance)
  invisible(x)
}
```

## S3 Methods

### print.sobol_generator

Provides a concise, user-friendly display of the generator state:
- Dimensions
- Initial skip value
- Current index
- Number of points generated

### summary.sobol_generator

Returns a structured summary object of class `summary.sobol_generator` containing:
- All metadata about the generator
- Current state information
- Computed statistics

### print.summary.sobol_generator

Formats the summary object for display with clear headers and alignment.

## Key Design Decisions

### 1. S3 vs R6 vs S4

**Chosen: S3**

Rationale:
- Simpler and more R-idiomatic
- Lower overhead
- Better integration with generic methods
- Easier for users to understand and extend
- Sufficient for this use case (no need for complex inheritance)

### 2. State Management

The Rcpp reference class maintains all mutable state. The S3 wrapper only stores:
- Reference to the Rcpp object (mutable)
- Metadata (immutable)

This hybrid approach provides:
- Efficient C++ state management
- R-friendly interface
- Clear separation of concerns

### 3. Return Values

Following CRAN policies and R best practices:
- Query functions return values directly
- Generation functions return numeric vectors/matrices
- Side-effect functions return objects invisibly
- Never return raw NULL (always return something meaningful)

### 4. Error Handling

Three layers of error handling:
1. R-level validation (type checking, range checking)
2. Rcpp try-catch blocks
3. C++ exception handling

All errors are converted to R errors with clear messages.

### 5. Documentation

Using Roxygen2 format:
- Full function documentation
- Parameter descriptions with types and constraints
- Return value specifications
- Examples (in `\dontrun{}` blocks since package may not be installed)
- Cross-references between related functions

## File Organization

```
sobol/
├── DESCRIPTION          # Package metadata
├── NAMESPACE            # Export declarations
├── R/
│   ├── sobol-package.R      # Package documentation and .onLoad
│   ├── sobol_generator.R    # S3 class and methods
│   └── sobol_points.R       # Batch function documentation
├── inst/
│   └── examples/
│       └── usage_examples.R # Comprehensive usage examples
└── tests/
    ├── testthat.R
    └── testthat/
        └── test-sobol_generator.R  # Comprehensive test suite
```

## Testing Strategy

The test suite validates:

1. **Construction**: Valid and invalid parameter combinations
2. **Point Generation**: Single and batch generation
3. **State Management**: Index tracking and skip functionality
4. **Methods**: Print and summary output
5. **Reproducibility**: Same parameters produce same sequences
6. **Edge Cases**: Large dimensions, large skip values, n=0
7. **Error Handling**: Proper validation and error messages
8. **Consistency**: Batch vs incremental generation equivalence

## Usage Patterns

### Basic Usage
```r
library(sobol)

# Create generator
gen <- sobol_generator(dimensions = 3)

# Generate points
point <- sobol_next(gen)
batch <- sobol_next_n(gen, n = 100)
```

### Reproducibility
```r
# Start from specific index
gen <- sobol_generator(dimensions = 2, skip = 1000)

# Or skip later
gen <- sobol_generator(dimensions = 2)
sobol_skip_to(gen, 1000)
```

### Inspection
```r
# Quick look
print(gen)

# Detailed information
summary(gen)

# Query state
idx <- sobol_index(gen)
dims <- sobol_dimensions(gen)
```

## Compliance

### CRAN Policies

- ✅ All functions return meaningful values (no bare NULL)
- ✅ Proper error handling with informative messages
- ✅ Documentation for all exported functions
- ✅ Examples provided (in \dontrun blocks)
- ✅ S3 methods properly registered in NAMESPACE
- ✅ No modification of global state
- ✅ Clean separation of C++ and R code

### R Best Practices

- ✅ Consistent naming conventions (snake_case)
- ✅ Proper parameter validation
- ✅ Generic methods for S3 classes
- ✅ Invisible returns for side-effect functions
- ✅ Informative print methods
- ✅ Comprehensive test coverage

## Performance Considerations

- C++ core handles all computation
- Minimal R overhead (just validation and dispatch)
- Column-major layout for efficient R matrix operations
- Reference semantics for state (no copying)
- Batch generation optimized for large n

## Future Extensions

Possible enhancements:
- `plot.sobol_generator()`: Visualize point distribution
- `as.data.frame.sobol_generator()`: Convert to data frame
- `[.sobol_generator()`: Subset/indexing operations
- `length.sobol_generator()`: Return current index
- `dim.sobol_generator()`: Return dimensions (for consistency)

## Conclusion

This S3 implementation provides:
- Clean, idiomatic R interface
- Full access to C++ performance
- Comprehensive documentation and tests
- CRAN-compliant design
- User-friendly API for both basic and advanced use cases
