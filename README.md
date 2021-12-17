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
25.6.12 | std::is_permutation | ❌ | ❌
25.6.13 | std::search | ✅ | ✅
| | std::search_n | ✅ | ✅
