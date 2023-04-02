#define PSTLD_HACK_INTO_STD
#include <pstld/pstld.h>

// https://en.cppreference.com/w/cpp/utility/feature_test

#ifndef __cpp_lib_execution
#error __cpp_lib_execution should be defined
#endif

#ifndef __cpp_lib_parallel_algorithm
#error __cpp_lib_parallel_algorithm should be defined
#endif

int main(){}
