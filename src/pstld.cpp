// Copyright (c) 2021 Michael G. Kazakov. All rights reserved. Distributed under the MIT License.
#include <pstld/pstld.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <dispatch/dispatch.h>

namespace pstld::internal {

size_t max_hw_threads() noexcept
{
    static const size_t threads = [] {
        int count;
        size_t count_len = sizeof(count);
        sysctlbyname("hw.logicalcpu_max", &count, &count_len, nullptr, 0);
        return static_cast<size_t>(count);
    }();
    return threads;
}

void dispatch_apply(size_t iterations, void *ctx, void (*function)(void *, size_t)) noexcept
{
    ::dispatch_apply_f(iterations, DISPATCH_APPLY_AUTO, ctx, function);
}

const char *parallelism_exception::what() const noexcept
{
    return "Failed to acquire resources to perform parallel computation";
}

void parallelism_exception::raise()
{
    throw parallelism_exception{};
};

} // namespace pstld::internal
