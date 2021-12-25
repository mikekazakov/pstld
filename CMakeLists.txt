cmake_minimum_required(VERSION 3.19)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(pstld VERSION 1.0
              DESCRIPTION "ParallelSTL on top of GCD"
              LANGUAGES CXX)

add_library(pstld
  pstld/pstld.h
  pstld/pstld.cpp
)

target_include_directories(pstld PUBLIC .)
set_property(TARGET pstld PROPERTY CXX_STANDARD 17)

add_subdirectory(benchmark)

if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(test-llvm-pstl)
    add_subdirectory(test-msvc-stl)
endif (BUILD_TESTING)
