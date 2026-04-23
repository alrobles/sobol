cmake_minimum_required(VERSION 3.16)
project(sobol LANGUAGES CXX)

add_library(sobol INTERFACE)
target_include_directories(sobol INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inst/include)
target_compile_features(sobol INTERFACE cxx_std_17)

# Code generator for precomputed tables
add_executable(generate_tables tools/generate_tables.cpp)
target_link_libraries(generate_tables PRIVATE sobol)

# Custom target to generate the precomputed tables header
add_custom_target(generate_sobol_tables
  COMMAND generate_tables ${CMAKE_CURRENT_SOURCE_DIR}/inst/include/sobol/precomputed_tables.hpp
  DEPENDS generate_tables
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Generating precomputed Sobol direction numbers and primitive polynomials"
  VERBATIM
)

enable_testing()

# Basic test suite
add_executable(test_sobol tests/test_sobol.cpp)
target_link_libraries(test_sobol PRIVATE sobol)
if(MSVC)
  target_compile_options(test_sobol PRIVATE /UNDEBUG)
else()
  target_compile_options(test_sobol PRIVATE -UNDEBUG)
endif()
add_test(NAME sobol_core_test COMMAND test_sobol)

# Enhanced comprehensive test suite
add_executable(test_sobol_enhanced tests/test_sobol_enhanced.cpp)
target_link_libraries(test_sobol_enhanced PRIVATE sobol)
if(MSVC)
  target_compile_options(test_sobol_enhanced PRIVATE /UNDEBUG)
else()
  target_compile_options(test_sobol_enhanced PRIVATE -UNDEBUG)
endif()
add_test(NAME sobol_enhanced_test COMMAND test_sobol_enhanced)

# Optional: Build performance benchmarks if Google Benchmark is available
option(BUILD_BENCHMARKS "Build performance benchmarks" ON)
if(BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()
