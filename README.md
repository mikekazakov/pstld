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
25.8.2.2 | std::stable_sort | ✅ | ✅
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
The measurements were done on a M1 MacBook Pro (4P+4E cores CPU) running macOS 13.

```
                                 1K   10K  100K    1M   10M  100M 
all_of                         0.14  0.34  1.37  2.70  2.38  2.64 
any_of                         0.51  0.76  0.77  1.84  2.19  2.67 
none_of                        0.52  0.71  0.74  1.84  2.10  2.56 
for_each                       0.24  0.74  0.83  0.85  0.99  1.19 
find                           0.21  0.72  0.76  1.76  2.14  2.57 
find_end                       0.13  0.15  0.30  0.63  0.69  0.80 
find_first_of                  0.37  0.96  1.97  4.68  5.34  5.41 
adjacent_find                  0.43  0.47  0.54  1.67  2.01  2.22 
count                          0.16  0.73  0.68  0.70  0.94  1.33 
mismatch                       0.29  0.67  0.72  1.10  1.14  1.17 
equal                          0.52  0.78  0.83  1.23  1.25  1.29 
search                         0.53  0.69  1.39  2.95  3.43  3.46 
copy                           0.29  0.79  0.39  0.77  0.97  1.43 
move                           0.60  0.41  0.96  1.07  0.98  1.55 
swap_ranges                    0.31  0.64  0.63  0.83  0.97  1.33 
transform                      0.32  0.85  0.60  0.72  0.83  1.48 
replace                        0.22  0.30  0.35  1.03  0.99  1.34 
fill                           0.09  0.64  0.44  0.80  0.71  1.03 
reverse                        0.27  0.81  0.57  1.34  1.02  0.98 
sort_Rnd                       0.14  1.71  2.97  3.73  4.35  4.31 
sort_Eq                        0.08  0.43  0.62  0.95  0.98  0.97 
sort_Asc                       0.07  0.08  0.29  0.40  0.43  0.39 
sort_Des                       0.38  0.32  0.64  0.94  0.98  0.97 
stable_sort                    0.61  1.52  3.25  3.70  4.05  4.17 
is_sorted                      0.30  0.41  0.79  1.89  2.10  2.73 
is_partitioned                 0.33  0.45  0.92  1.91  2.10  2.74 
merge                          1.00  0.31  0.82  1.28  1.29  1.74 
minmax_element                 1.44  1.18  2.32  5.97  7.27  7.26 
lexicographical_compare        0.64  0.62  1.31  2.14  2.11  2.20 
reduce                         1.11  1.11  1.53  4.27  5.58  5.59 
transform_reduce               1.08  1.55  1.45  4.28  5.63  5.67 
exclusive_scan                 0.63  0.70  0.68  2.22  2.24  2.42 
inclusive_scan                 0.69  0.50  0.82  2.32  2.24  2.44 
transform_exclusive_scan       0.46  0.49  1.70  2.26  2.30  2.31 
transform_inclusive_scan       0.40  0.40  1.70  2.27  2.33  2.34 
adjacent_difference            0.29  0.82  0.49  0.76  0.79  0.78 
uninitialized_value_construct  0.26  0.42  0.49  0.90  0.99  1.70 
uninitialized_copy             0.59  0.66  1.74  1.38  1.44  2.08 
uninitialized_move             0.40  0.40  1.08  1.24  1.17  1.70 
uninitialized_fill             0.54  1.04  3.26  4.99  5.30  5.14 
destroy                        0.17  0.67  1.47  1.61  1.60  1.69 
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