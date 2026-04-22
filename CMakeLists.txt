cmake_minimum_required(VERSION 3.16)
project(sobol LANGUAGES CXX)

add_library(sobol INTERFACE)
target_include_directories(sobol INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(sobol INTERFACE cxx_std_17)

enable_testing()
add_executable(test_sobol tests/test_sobol.cpp)
target_link_libraries(test_sobol PRIVATE sobol)
add_test(NAME sobol_core_test COMMAND test_sobol)
