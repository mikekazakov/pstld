![Build Status](https://github.com/mikekazakov/pstld/actions/workflows/build.yml/badge.svg)

# pstld
Experimental implementation of ParallelSTL on top of Grand Central Dispatch aka libdispatch

The purpose of this library is to provide a drop-in implementation of C++ parallel algorithms for the Apple platforms.
Xcode comes with no parallel algorithms in libc++, so this library aims to fill the gap.
pstld uses the native scheduler (libdispatch) and does not depend on any 3rd-party libraries.

## Usage

The simplest way to use the library is to consume it as header-only and to request exposing the parallel algorithms in the namespace ```std```:
```C++
#include <cstdlib>
#include <iostream>
#define PSTLD_HEADER_ONLY   // no prebuilt library, only the header
#define PSTLD_HACK_INTO_STD // export into namespace std
#include "pstld.h"
int main()
{
    std::vector<unsigned> v(100'000'000);
    std::generate(v.begin(), v.end(), [] { return std::rand(); });
    std::cout << std::reduce(std::execution::par, v.begin(), v.end()) << std::endl;
}
```
The default Xcode toolchain can now build it:
```Shell
% clang++ -std=c++17 main.cpp -o test && ./test
64818392
```
The same functions can be used directly from the ```pstld``` namespace without specifying the execution policy:
```C++
#include <cstdlib>
#include <iostream>
#define PSTLD_HEADER_ONLY   // no prebuilt library, only the header
#include "pstld.h"
int main()
{
    std::vector<unsigned> v(100'000'000);
    std::generate(v.begin(), v.end(), [] { return std::rand(); });
    std::cout << pstld::reduce(v.begin(), v.end()) << std::endl;
}
```
pstld can be precompiled via CMake or manually by adding ```pstld.cpp``` into the build sources:
```C++
#include <cstdlib>
#include <iostream>
#include "pstld.h"
int main()
{
    std::vector<unsigned> v(100'000'000);
    std::generate(v.begin(), v.end(), [] { return std::rand(); });
    std::cout << pstld::reduce(v.begin(), v.end()) << std::endl;
}
```
```Shell
% clang++ -std=c++17 main.cpp pstld.cpp -o test && ./test
64818392
```

## Completeness

The library is not complete, this table shows which algorithms are currently available:

§ | Function | Exposed | Parallel
:--- |:--- |:---:|:---:
25.6.1 | std::all_of | ✅ | ✅
25.6.2 | std::any_of | ✅ | ✅
25.6.3 | std::none_of | ✅ | ✅
25.6.4 | std::for_each | ✅ | ✅
| | std::for_each_n | ✅ | ✅
25.6.5 | std::find | ✅ | ✅
| | std::find_if | ✅ | ✅
| | std::find_if_not | ✅ | ✅
25.6.6 | std::find_end | ✅ | ✅
25.6.7 | std::find_first_of | ✅ | ✅
25.6.8 | std::adjacent_find | ✅ | ✅
25.6.9 | std::count | ✅ | ✅
| | std::count_if | ✅ | ✅
25.6.10 | std::mismatch | ✅ | ✅
25.6.11 | std::equal | ✅ | ✅
25.6.13 | std::search | ✅ | ✅
| | std::search_n | ✅ | ✅
25.7.1 | std::copy | ✅ | ✅
| | std::copy_n | ✅ | ✅
| | std::copy_if | ✅ | ❌
25.7.2 | std::move | ✅ | ✅
25.7.3 | std::swap_ranges | ✅ | ✅
25.7.4 | std::transform | ✅ | ✅
25.7.5 | std::replace | ✅ | ✅
| | std::replace_if | ✅ | ✅
| | std::replace_copy | ✅ | ❌
| | std::replace_copy_if | ✅ | ❌
25.7.6 | std::fill | ✅ | ✅
| | std::fill_n | ✅ | ✅
25.7.7 | std::generate | ✅ | ❌
| | std::generate_n | ✅ | ❌
25.7.8 | std::remove | ❌ | ❌
| | std::remove_if | ❌ | ❌
| | std::remove_copy | ❌ | ❌
| | std::remove_copy_if | ❌ | ❌
25.7.9 | std::unique | ❌ | ❌
| | std::unique_copy | ❌ | ❌
25.7.10 | std::reverse | ✅ | ✅
| | std::reverse_copy | ❌ | ❌
25.7.11 | std::rotate | ❌ | ❌
25.7.14 | std::shift_left | ❌ | ❌
| | std::shift_right | ❌ | ❌
25.8.2.1 | std::sort | ✅ | ✅
25.8.2.2 | std::stable_sort | ✅ | ❌
25.8.2.3 | std::partial_sort | ❌ | ❌
25.8.2.4 | std::partial_sort_copy | ❌ | ❌
25.8.2.5 | std::is_sorted | ✅ | ✅
| | std::is_sorted_until | ✅ | ✅
25.8.3 | std::nth_element | ❌ | ❌
25.8.5 | std::is_partitioned | ✅ | ✅
| | std::partition | ❌ | ❌
| | std::stable_partition | ❌ | ❌
| | std::partition_copy | ❌ | ❌
25.8.6 | std::merge | ✅ | ✅
| | std::inplace_merge | ❌ | ❌
25.8.7.2 | std::includes | ❌ | ❌
25.8.7.3 | std::set_union | ❌ | ❌
25.8.7.4 | std::set_intersection | ❌ | ❌
25.8.7.5 | std::set_difference | ❌ | ❌
25.8.7.6 | std::set_symmetric_difference | ❌ | ❌
25.8.8.6 | std::is_heap | ❌ | ❌
| | std::is_heap_until | ❌ | ❌
25.8.9 | std::min_element | ✅ | ✅
| | std::max_element | ✅ | ✅
| | std::minmax_element | ✅ | ✅
25.8.11 | std::lexicographical_compare | ✅ | ✅
25.10.4 | std::reduce | ✅ | ✅
25.10.6 | std::transform_reduce | ✅ | ✅
25.10.8 | std::exclusive_scan | ✅ | ✅
25.10.9 | std::inclusive_scan | ✅ | ✅
25.10.10 | std::transform_exclusive_scan | ✅ | ✅
25.10.11 | std::transform_inclusive_scan | ✅ | ✅
25.10.12 | std::adjacent_difference | ✅ | ✅
25.11.3 | std::uninitialized_default_construct | ✅ | ✅
| | std::uninitialized_default_construct_n | ✅ | ✅
25.11.4 | std::uninitialized_value_construct | ✅ | ✅
| | std::uninitialized_value_construct_n | ✅ | ✅
25.11.5 | std::uninitialized_copy | ✅ | ✅
| | std::uninitialized_copy_n | ✅ | ✅
25.11.6 | std::uninitialized_move | ✅ | ✅
| | std::uninitialized_move_n | ✅ | ✅
25.11.7 | std::uninitialized_fill | ✅ | ✅
| | std::uninitialized_fill_n | ✅ | ✅
25.11.9 | std::destroy | ✅ | ✅
| | std::destroy_n | ✅ | ✅

## Parallel speedup

```benchmark.cpp``` contains a set of primitive synthetic performance tests.
Each row shows how the parallel implementation compares to its serial counterpart depending on the number of elements in a working set.
The serial variant executes the default algorithms from libc++ and the parallel one runs the pstld implementation.
Per-element operations are mostly trivial in these benchmarks, so the speed-up numbers represent a somewhat worst-case scenario.
The measurements were done on a M1 MacBook Pro (4P+4E cores CPU).

```
                               1000      10000     100000    1000000   10000000  100000000 
all_of                         0.80       1.07       2.56       5.08       4.23       5.64 
any_of                         0.47       0.72       0.86       1.90       2.29       3.56 
none_of                        0.45       0.71       0.78       1.74       2.13       3.43 
for_each                       0.22       0.76       0.43       0.75       0.97       1.90 
find                           0.45       0.74       0.76       1.73       2.12       2.99 
find_end                       0.12       0.15       0.23       0.67       0.73       1.19 
find_first_of                  0.72       0.97       1.73       4.74       4.89       5.28 
adjacent_find                  0.45       0.51       0.63       1.68       1.99       2.23 
count                          0.07       0.37       0.47       0.71       0.93       2.34 
mismatch                       0.30       0.67       0.68       1.10       1.10       1.51 
equal                          0.50       0.80       0.76       1.23       1.23       1.27 
search                         0.50       0.83       1.61       4.22       4.39       4.78 
copy                           0.29       0.74       0.37       0.73       0.96       1.87 
move                           0.18       1.09       2.22       1.15       1.11       1.76 
swap_ranges                    0.35       0.75       0.70       0.80       0.99       1.69 
transform                      0.14       0.84       0.44       0.76       0.84       2.30 
replace                        0.12       0.31       0.32       0.85       1.05       1.83 
fill                           0.06       0.31       0.44       0.71       0.74       1.32 
reverse                        0.15       0.29       0.55       1.26       1.01       0.98 
sort_Rnd                       0.14       1.73       3.05       3.71       4.32       4.28 
sort_Eq                        0.08       0.39       0.67       0.90       0.98       1.01 
sort_Asc                       0.09       0.10       0.33       0.41       0.43       0.38 
sort_Des                       0.08       0.28       0.64       0.95       0.98       1.01 
is_sorted                      0.19       0.55       0.66       1.57       2.04       2.99 
is_partitioned                 0.31       0.62       0.80       1.61       2.14       3.17 
merge                          0.99       0.50       0.75       1.36       1.96       2.02 
minmax_element                 1.02       0.88       2.60       5.94       7.22       6.96 
lexicographical_compare        0.69       1.28       1.47       2.15       2.11       2.21 
reduce                         0.56       1.17       1.65       4.27       5.57       5.58 
transform_reduce               0.86       1.29       1.41       4.33       5.24       5.52 
exclusive_scan                 0.56       0.68       0.77       2.33       2.19       2.39 
inclusive_scan                 0.52       0.62       0.93       2.26       2.24       2.44 
transform_exclusive_scan       0.44       0.44       1.68       2.24       2.27       2.23 
transform_inclusive_scan       0.39       0.57       1.77       2.27       2.29       2.32 
adjacent_difference            0.21       0.75       0.49       0.91       0.79       0.78
uninitialized_value_construct  0.44       0.27       0.92       0.98       1.70       2.27
uninitialized_copy             0.51       0.99       3.39       1.69       2.25       2.10
uninitialized_move             0.54       0.47       1.16       1.22       1.65       1.79
uninitialized_fill             0.68       1.55       4.57       4.93       4.73       4.91
destroy                        0.25       0.64       1.51       1.61       1.67       1.92
```

## Running the benchmarks

Run the following commands to execute the benchmarks locally:

```Shell
git clone https://github.com/mikekazakov/pstld.git && \
  cd pstld && \
  cd test-llvm-pstl && ./bootstrap.sh && cd - && \
  cd test-msvc-stl && ./bootstrap.sh && cd - && \
  cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release . && \
  make benchmark &&
  ./benchmark/benchmark
```