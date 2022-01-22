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
25.8.6 | std::merge | ❌ | ❌
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
