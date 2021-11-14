cmake_minimum_required(VERSION 3.19)

add_executable(benchmark
    benchmark.cpp
)

target_link_libraries(benchmark PRIVATE pstld)
set_property(TARGET benchmark PROPERTY CXX_STANDARD 17)
set_target_properties(benchmark PROPERTIES COMPILE_FLAGS "-include${CMAKE_CURRENT_LIST_DIR}/../include/pstld/hack_into_std.h")
