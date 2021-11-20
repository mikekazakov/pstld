cmake_minimum_required(VERSION 3.19)

project(pstld VERSION 1.0
              DESCRIPTION "ParallelSTL on top of GCD"
              LANGUAGES CXX)

add_library(pstld
  include/pstld/pstld.h
  include/pstld/hack_into_std.h
  src/pstld.cpp
)

target_include_directories(pstld PUBLIC include)
set_property(TARGET pstld PROPERTY CXX_STANDARD 17)

add_subdirectory(benchmark)

enable_testing()
add_subdirectory(test-llvm-pstl)
add_subdirectory(test-msvc-stl)
