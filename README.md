![Build Status](https://github.com/mikekazakov/pstld/actions/workflows/build.yml/badge.svg)

# pstld
Experimental implementation of ParallelSTL on top of GCD aka libdispatch

## Completeness

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
25.7.2 | std::move | ✅ | ❌
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
25.8.5 | std::is_partitioned | ❌ | ❌
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
25.11.3 | std::uninitialized_default_construct | ❌ | ❌
| | std::uninitialized_default_construct_n | ❌ | ❌
25.11.4 | std::uninitialized_value_construct | ❌ | ❌
| | std::uninitialized_value_construct_n | ❌ | ❌
25.11.5 | std::uninitialized_copy | ❌ | ❌
| | std::uninitialized_copy_n | ❌ | ❌
25.11.6 | std::uninitialized_move | ❌ | ❌
| | std::uninitialized_move_n | ❌ | ❌
25.11.7 | std::uninitialized_fill | ❌ | ❌
| | std::uninitialized_fill_n | ❌ | ❌
25.11.9 | std::destroy | ❌ | ❌
| | std::destroy_n | ❌ | ❌

## Parallel speed-up

benchmark.cpp contains a set of primitive synthetic performance tests.
Each row shows how the parallel implementation compares to its serial counterpart depending on the number of elements in a working set.
The serial variant executes the default algorithms from LIBC++ and the parallel one runs the PSTLD implementation.
Per-element operations are trivial in these benchmarks, so the speed-up numbers represent a somewhat worst-case scenario.
The measurements were done on a M1 MacBook Pro (8 core CPU).

```
                                100       1000      10000     100000    1000000   10000000  100000000 
all_of                         0.04       0.86       1.38       3.00       5.04       4.52       5.81 
any_of                         0.13       0.35       0.76       0.87       1.67       2.20       3.13 
none_of                        0.08       0.39       0.71       0.91       1.71       2.11       3.13 
for_each                       0.03       0.24       0.39       0.78       0.80       1.00       1.75 
find                           0.12       0.38       0.72       0.80       1.71       2.13       3.08 
find_end                       0.09       0.12       0.14       0.27       0.66       0.75       1.20 
find_first_of                  0.21       0.81       0.98       1.81       4.38       4.77       4.78 
adjacent_find                  0.10       0.40       0.51       0.68       1.61       1.96       2.18 
count                          0.00       0.09       0.72       0.73       0.88       0.93       2.24 
mismatch                       0.17       0.35       0.67       0.66       1.10       1.11       1.14 
equal                          0.09       0.39       0.78       0.81       1.33       1.22       1.27 
search                         0.11       0.59       0.67       1.46       3.40       3.79       4.07 
copy                           0.04       0.25       0.75       0.51       0.87       0.97       1.98 
swap_ranges                    0.14       0.31       0.82       0.60       0.83       0.97       1.65 
transform                      0.05       0.14       0.84       0.43       0.81       0.85       2.24 
replace                        0.07       0.19       0.32       0.39       0.93       1.04       1.87 
fill                           0.11       0.18       0.66       0.93       0.73       0.73       1.31 
reverse                        0.08       0.22       0.94       0.53       1.27       1.00       0.97 
sort_Rnd                       0.04       0.13       1.67       3.05       3.74       4.28       4.22 
sort_Eq                        0.01       0.10       0.26       0.65       0.96       0.98       1.00 
sort_Asc                       0.02       0.10       0.09       0.26       0.37       0.34       0.32 
sort_Des                       0.01       0.11       0.34       0.65       0.94       0.99       0.99 
is_sorted                      0.09       0.40       0.46       0.59       1.54       2.02       2.88 
merge                          0.92       1.00       0.52       0.83       1.31       1.83       2.00 
minmax_element                 0.23       1.12       1.24       2.28       5.65       6.54       6.75 
lexicographical_compare        0.23       0.65       1.23       1.39       2.16       2.07       2.17 
reduce                         0.12       0.86       1.45       1.31       4.06       5.52       5.40 
transform_reduce               0.08       0.92       1.29       1.16       4.10       5.49       5.49 
exclusive_scan                 0.10       0.82       0.58       0.91       2.19       2.21       2.40 
inclusive_scan                 0.11       0.74       0.67       0.79       2.30       2.21       2.42 
transform_exclusive_scan       0.30       0.51       0.40       1.68       2.19       2.25       2.29 
transform_inclusive_scan       0.25       0.49       0.48       1.50       2.25       2.29       2.31 
adjacent_difference            0.09       0.29       1.02       0.49       0.77       0.80       0.79 
```
