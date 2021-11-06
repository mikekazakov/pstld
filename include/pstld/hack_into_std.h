#pragma once
#include <algorithm>
#include <numeric>
#include <type_traits>
#include "pstld.h"

namespace std
{

namespace execution
{

class sequenced_policy {
    public:
    using __standard_execution_policy  = int;
    static constexpr bool __enabled = false;
};
class parallel_policy {
    public:
    using __standard_execution_policy  = int;
    static constexpr bool __enabled = true;
};
class parallel_unsequenced_policy {
    public:
    using __standard_execution_policy  = int;
    static constexpr bool __enabled = true;
};
class unsequenced_policy {
    public:
    using __standard_execution_policy  = int;
    static constexpr bool __enabled = false;
};

inline constexpr sequenced_policy seq;
inline constexpr parallel_policy par;
inline constexpr parallel_unsequenced_policy par_unseq;
inline constexpr unsequenced_policy unseq;

template <class T> struct is_execution_policy : std::false_type {};
template <> struct is_execution_policy<sequenced_policy> : std::true_type {};
template <> struct is_execution_policy<parallel_policy> : std::true_type {};
template <> struct is_execution_policy<parallel_unsequenced_policy> : std::true_type {};
template <> struct is_execution_policy<unsequenced_policy> : std::true_type {};

template <class T>
constexpr bool is_execution_policy_v = is_execution_policy<T>::value;

template <class ExPo, class T>
using __enable_if_execution_policy =
typename std::enable_if<is_execution_policy<typename std::decay<ExPo>::type>::value, T>::type;
}

template <class ExPo, class It>
execution::__enable_if_execution_policy<ExPo, typename iterator_traits<It>::value_type>
reduce(ExPo&&, const It first, const It last) noexcept
{
    using T = typename iterator_traits<It>::value_type;
    if constexpr (remove_reference_t<ExPo>::__enabled)
        return pstld::transform_reduce(first, last, T{}, std::plus<T>{}, pstld::internal::no_op{});
    else
        return std::reduce(first, last, T{}, std::plus<T>{});
}

template <class ExPo, class It, class T>
execution::__enable_if_execution_policy<ExPo, T>
reduce(ExPo&&, const It first, const It last, T val) noexcept
{
    if constexpr (remove_reference_t<ExPo>::__enabled)
        return pstld::transform_reduce(first, last, val, std::plus<T>{}, pstld::internal::no_op{});
    else
        return std::reduce(first, last, val, std::plus<T>{});
}

template <class ExPo, class It, class T, class BinOp>
execution::__enable_if_execution_policy<ExPo, T>
reduce(ExPo&&, const It first, const It last, T val, BinOp op) noexcept
{
    if constexpr (remove_reference_t<ExPo>::__enabled)
        return pstld::transform_reduce(first, last, val, op, pstld::internal::no_op{});
    else
        return std::reduce(first, last, val, op);
}

template <class ExPo, class It, class T, class BinOp, class UnOp>
execution::__enable_if_execution_policy<ExPo, T>
transform_reduce(ExPo&&, const It first, const It last, T val, BinOp bop, UnOp uop) noexcept
{
    if constexpr (remove_reference_t<ExPo>::__enabled)
        return pstld::transform_reduce(first, last, val, bop, uop);
    else
        return std::transform_reduce(first, last, val, bop, uop);
}

}

// a hack to suport running tests from LLVM's PSTL
namespace __pstl {
    namespace execution = ::std::execution;
}

