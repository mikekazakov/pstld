#pragma once

// a hack to suport running tests from LLVM's PSTL
namespace __pstl {
namespace execution = ::std::execution;
namespace __internal {
template <typename T>
struct __equal_value {
    const T &val;
    explicit __equal_value(const T &val) : val(val) {}
    template <typename U>
    bool operator()(U &&other) const
    {
        return std::forward<U>(other) == val;
    }
};
} // namespace __internal

} // namespace __pstl
