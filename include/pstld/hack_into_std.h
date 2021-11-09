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
    static constexpr bool __enabled = false;
};
class parallel_policy
{
public:
    static constexpr bool __enabled = true;
};
class parallel_unsequenced_policy
{
public:
    static constexpr bool __enabled = true;
};
class unsequenced_policy
{
public:
    static constexpr bool __enabled = false;
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
constexpr bool is_execution_policy_v = is_execution_policy<T>::value;

template <class ExPo, class T>
using __enable_if_execution_policy =
    typename std::enable_if<is_execution_policy<typename std::decay<ExPo>::type>::value, T>::type;

} // namespace execution

// 25.6.1 - all_of /////////////////////////////////////////////////////////////////////////////////

template <class ExPo, class It, class UnPred>
execution::__enable_if_execution_policy<ExPo, bool>
all_of(ExPo &&, It first, It last, UnPred p) noexcept
{
    if constexpr( remove_reference_t<ExPo>::__enabled )
        return ::pstld::all_of(first, last, p);
    else
        return ::std::all_of(first, last, p);
}

// 25.6.2 - any_of ////////////////////////////////////////////////////////////////////////////////

template <class ExPo, class It, class UnPred>
execution::__enable_if_execution_policy<ExPo, bool>
any_of(ExPo &&, It first, It last, UnPred p) noexcept
{
    if constexpr( remove_reference_t<ExPo>::__enabled )
        return ::pstld::any_of(first, last, p);
    else
        return ::std::any_of(first, last, p);
}

// 25.6.3 - none_of ////////////////////////////////////////////////////////////////////////////////

template <class ExPo, class It, class UnPred>
execution::__enable_if_execution_policy<ExPo, bool>
none_of(ExPo &&, It first, It last, UnPred p) noexcept
{
    if constexpr( remove_reference_t<ExPo>::__enabled )
        return ::pstld::none_of(first, last, p);
    else
        return ::std::none_of(first, last, p);
}

// 25.6.4 - for_each, for_each_n ///////////////////////////////////////////////////////////////////

template <class ExPo, class It, class Func>
execution::__enable_if_execution_policy<ExPo, void>
for_each(ExPo &&, It first, It last, Func f) noexcept
{
    if constexpr( remove_reference_t<ExPo>::__enabled )
        ::pstld::for_each(first, last, f);
    else
        ::std::for_each(first, last, f);
}

template <class ExPo, class It, class Size, class Func>
execution::__enable_if_execution_policy<ExPo, void>
for_each_n(ExPo &&, It first, Size count, Func f) noexcept
{
    if constexpr( remove_reference_t<ExPo>::__enabled )
        ::pstld::for_each_n(first, count, f);
    else
        ::std::for_each_n(first, count, f);
}

// 25.10.4 - reduce ////////////////////////////////////////////////////////////////////////////////

template <class ExPo, class It>
execution::__enable_if_execution_policy<ExPo, typename iterator_traits<It>::value_type>
reduce(ExPo &&, It first, It last) noexcept
{
    if constexpr( remove_reference_t<ExPo>::__enabled )
        return ::pstld::reduce(first, last);
    else
        return ::std::reduce(first, last);
}

template <class ExPo, class It, class T>
execution::__enable_if_execution_policy<ExPo, T> reduce(ExPo &&, It first, It last, T val) noexcept
{
    if constexpr( remove_reference_t<ExPo>::__enabled )
        return ::pstld::reduce(first, last, val);
    else
        return ::std::reduce(first, last, val);
}

template <class ExPo, class It, class T, class BinOp>
execution::__enable_if_execution_policy<ExPo, T>
reduce(ExPo &&, It first, It last, T val, BinOp op) noexcept
{
    if constexpr( remove_reference_t<ExPo>::__enabled )
        return ::pstld::reduce(first, last, val, op);
    else
        return ::std::reduce(first, last, val, op);
}

// 25.10.6 - transform_reduce //////////////////////////////////////////////////////////////////////

template <class ExPo, class It1, class It2, class T>
execution::__enable_if_execution_policy<ExPo, T>
transform_reduce(ExPo &&, It1 first1, It1 last1, It2 first2, T val) noexcept
{
    if constexpr( remove_reference_t<ExPo>::__enabled )
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
    if constexpr( remove_reference_t<ExPo>::__enabled )
        return ::pstld::transform_reduce(first1, last1, first2, val, redop, trop);
    else
        return ::std::transform_reduce(first1, last1, first2, val, redop, trop);
}

template <class ExPo, class It, class T, class BinOp, class UnOp>
execution::__enable_if_execution_policy<ExPo, T>
transform_reduce(ExPo &&, It first, It last, T val, BinOp bop, UnOp uop) noexcept
{
    if constexpr( remove_reference_t<ExPo>::__enabled )
        return ::pstld::transform_reduce(first, last, val, bop, uop);
    else
        return ::std::transform_reduce(first, last, val, bop, uop);
}

} // namespace std

// a hack to suport running tests from LLVM's PSTL
namespace __pstl {
namespace execution = ::std::execution;
}
