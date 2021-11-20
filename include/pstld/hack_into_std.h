// Copyright (c) 2021 Michael G. Kazakov. All rights reserved. Distributed under the MIT License.
#pragma once
#include <algorithm>
#include <numeric>
#include <type_traits>
#include "pstld.h"

namespace std {

namespace execution {

class sequenced_policy
{
public:
    static constexpr bool __pstld_enabled = false;
};
class parallel_policy
{
public:
    static constexpr bool __pstld_enabled = true;
};
class parallel_unsequenced_policy
{
public:
    static constexpr bool __pstld_enabled = true;
};
class unsequenced_policy
{
public:
    static constexpr bool __pstld_enabled = false;
};

inline constexpr sequenced_policy seq;
inline constexpr parallel_policy par;
inline constexpr parallel_unsequenced_policy par_unseq;
inline constexpr unsequenced_policy unseq;

template <class T>
struct is_execution_policy : std::false_type {
};
template <>
struct is_execution_policy<sequenced_policy> : std::true_type {
};
template <>
struct is_execution_policy<parallel_policy> : std::true_type {
};
template <>
struct is_execution_policy<parallel_unsequenced_policy> : std::true_type {
};
template <>
struct is_execution_policy<unsequenced_policy> : std::true_type {
};

template <class T>
inline constexpr bool is_execution_policy_v = is_execution_policy<T>::value;

template <class ExPo, class T>
using __enable_if_execution_policy =
    typename std::enable_if<is_execution_policy<typename std::decay<ExPo>::type>::value, T>::type;

template <class ExPo>
inline constexpr bool __pstld_enabled = std::remove_reference_t<ExPo>::__pstld_enabled;

} // namespace execution

// 25.6.1 - all_of /////////////////////////////////////////////////////////////////////////////////

template <class ExPo, class It, class UnPred>
execution::__enable_if_execution_policy<ExPo, bool>
all_of(ExPo &&, It first, It last, UnPred p) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::all_of(first, last, p);
    else
        return ::std::all_of(first, last, p);
}

// 25.6.2 - any_of ////////////////////////////////////////////////////////////////////////////////

template <class ExPo, class It, class UnPred>
execution::__enable_if_execution_policy<ExPo, bool>
any_of(ExPo &&, It first, It last, UnPred p) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::any_of(first, last, p);
    else
        return ::std::any_of(first, last, p);
}

// 25.6.3 - none_of ////////////////////////////////////////////////////////////////////////////////

template <class ExPo, class It, class UnPred>
execution::__enable_if_execution_policy<ExPo, bool>
none_of(ExPo &&, It first, It last, UnPred p) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::none_of(first, last, p);
    else
        return ::std::none_of(first, last, p);
}

// 25.6.4 - for_each, for_each_n ///////////////////////////////////////////////////////////////////

template <class ExPo, class It, class Func>
execution::__enable_if_execution_policy<ExPo, void>
for_each(ExPo &&, It first, It last, Func f) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        ::pstld::for_each(first, last, f);
    else
        ::std::for_each(first, last, f);
}

template <class ExPo, class It, class Size, class Func>
execution::__enable_if_execution_policy<ExPo, It>
for_each_n(ExPo &&, It first, Size count, Func f) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::for_each_n(first, count, f);
    else
        return ::std::for_each_n(first, count, f);
}

// 25.6.5 - find, find_if, find_if_not /////////////////////////////////////////////////////////////

template <class ExPo, class It, class T>
execution::__enable_if_execution_policy<ExPo, It>
find(ExPo &&, It first, It last, const T &value) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::find(first, last, value);
    else
        return ::std::find(first, last, value);
}

template <class ExPo, class It, class Pred>
execution::__enable_if_execution_policy<ExPo, It>
find_if(ExPo &&, It first, It last, Pred pred) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::find_if(first, last, pred);
    else
        return ::std::find_if(first, last, pred);
}

template <class ExPo, class It, class Pred>
execution::__enable_if_execution_policy<ExPo, It>
find_if_not(ExPo &&, It first, It last, Pred pred) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::find_if_not(first, last, pred);
    else
        return ::std::find_if_not(first, last, pred);
}

// 25.6.6 - find_end ///////////////////////////////////////////////////////////////////////////////

template <class ExPo, class It1, class It2>
execution::__enable_if_execution_policy<ExPo, It1>
find_end(ExPo &&, It1 first1, It1 last1, It2 first2, It2 last2)
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::find_end(first1, last1, first2, last2);
    else
        return ::std::find_end(first1, last1, first2, last2);
}

template <class ExPo, class It1, class It2, class Pred>
execution::__enable_if_execution_policy<ExPo, It1>
find_end(ExPo &&, It1 first1, It1 last1, It2 first2, It2 last2, Pred pred)
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::find_end(first1, last1, first2, last2, pred);
    else
        return ::std::find_end(first1, last1, first2, last2, pred);
}

// 25.6.7 - find_first_of //////////////////////////////////////////////////////////////////////////

template <class ExPo, class It1, class It2>
execution::__enable_if_execution_policy<ExPo, It1>
find_first_of(ExPo &&, It1 first1, It1 last1, It2 first2, It2 last2) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::find_first_of(first1, last1, first2, last2);
    else
        return ::std::find_first_of(first1, last1, first2, last2);
}

template <class ExPo, class It1, class It2, class Pred>
execution::__enable_if_execution_policy<ExPo, It1>
find_first_of(ExPo &&, It1 first1, It1 last1, It2 first2, It2 last2, Pred pred) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::find_first_of(first1, last1, first2, last2, pred);
    else
        return ::std::find_first_of(first1, last1, first2, last2, pred);
}

// 25.6.8 - adjacent_find //////////////////////////////////////////////////////////////////////////

template <class ExPo, class It>
execution::__enable_if_execution_policy<ExPo, It> adjacent_find(ExPo &&, It first, It last) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::adjacent_find(first, last);
    else
        return ::std::adjacent_find(first, last);
}

template <class ExPo, class It, class Pred>
execution::__enable_if_execution_policy<ExPo, It>
adjacent_find(ExPo &&, It first, It last, Pred pred) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::adjacent_find(first, last, pred);
    else
        return ::std::adjacent_find(first, last, pred);
}

// 25.6.9 - count, count_if ////////////////////////////////////////////////////////////////////////

template <class ExPo, class It, class T>
execution::__enable_if_execution_policy<ExPo, typename std::iterator_traits<It>::difference_type>
count(ExPo &&, It first, It last, const T &value) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::count(first, last, value);
    else
        return ::std::count(first, last, value);
}

template <class ExPo, class It, class Pred>
execution::__enable_if_execution_policy<ExPo, typename std::iterator_traits<It>::difference_type>
count_if(ExPo &&, It first, It last, Pred pred) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::count_if(first, last, pred);
    else
        return ::std::count_if(first, last, pred);
}

// 25.6.13 - search ////////////////////////////////////////////////////////////////////////////////

template <class ExPo, class It1, class It2>
execution::__enable_if_execution_policy<ExPo, It1>
search(ExPo &&, It1 first1, It1 last1, It2 first2, It2 last2) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::search(first1, last1, first2, last2);
    else
        return ::std::search(first1, last1, first2, last2);
}

template <class ExPo, class It1, class It2, class Pred>
execution::__enable_if_execution_policy<ExPo, It1>
search(ExPo &&, It1 first1, It1 last1, It2 first2, It2 last2, Pred pred) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::search(first1, last1, first2, last2, pred);
    else
        return ::std::search(first1, last1, first2, last2, pred);
}

// 25.10.4 - reduce ////////////////////////////////////////////////////////////////////////////////

template <class ExPo, class It>
execution::__enable_if_execution_policy<ExPo, typename iterator_traits<It>::value_type>
reduce(ExPo &&, It first, It last) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::reduce(first, last);
    else
        return ::std::reduce(first, last);
}

template <class ExPo, class It, class T>
execution::__enable_if_execution_policy<ExPo, T> reduce(ExPo &&, It first, It last, T val) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::reduce(first, last, val);
    else
        return ::std::reduce(first, last, val);
}

template <class ExPo, class It, class T, class BinOp>
execution::__enable_if_execution_policy<ExPo, T>
reduce(ExPo &&, It first, It last, T val, BinOp op) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::reduce(first, last, val, op);
    else
        return ::std::reduce(first, last, val, op);
}

// 25.10.6 - transform_reduce //////////////////////////////////////////////////////////////////////

template <class ExPo, class It1, class It2, class T>
execution::__enable_if_execution_policy<ExPo, T>
transform_reduce(ExPo &&, It1 first1, It1 last1, It2 first2, T val) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::transform_reduce(first1, last1, first2, val);
    else
        return ::std::transform_reduce(first1, last1, first2, val);
}

template <class ExPo, class It1, class It2, class T, class BinRedOp, class BinTrOp>
execution::__enable_if_execution_policy<ExPo, T> transform_reduce(ExPo &&,
                                                                  It1 first1,
                                                                  It1 last1,
                                                                  It2 first2,
                                                                  T val,
                                                                  BinRedOp redop,
                                                                  BinTrOp trop) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::transform_reduce(first1, last1, first2, val, redop, trop);
    else
        return ::std::transform_reduce(first1, last1, first2, val, redop, trop);
}

template <class ExPo, class It, class T, class BinOp, class UnOp>
execution::__enable_if_execution_policy<ExPo, T>
transform_reduce(ExPo &&, It first, It last, T val, BinOp bop, UnOp uop) noexcept
{
    if constexpr( execution::__pstld_enabled<ExPo> )
        return ::pstld::transform_reduce(first, last, val, bop, uop);
    else
        return ::std::transform_reduce(first, last, val, bop, uop);
}

} // namespace std

// a hack to suport running tests from LLVM's PSTL
namespace __pstl {
namespace execution = ::std::execution;
}
