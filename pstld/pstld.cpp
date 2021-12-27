// Copyright (c) 2021 Michael G. Kazakov. All rights reserved. Distributed under the MIT License.
#include <pstld/pstld.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <dispatch/dispatch.h>

namespace pstld::internal {

PSTLD_INTERNAL_IMPL size_t max_hw_threads() noexcept
{
    static const size_t threads = [] {
        int count;
        size_t count_len = sizeof(count);
        sysctlbyname("hw.logicalcpu_max", &count, &count_len, nullptr, 0);
        return static_cast<size_t>(count);
    }();
    return threads;
}

PSTLD_INTERNAL_IMPL void
dispatch_apply(size_t iterations, void *ctx, void (*function)(void *, size_t)) noexcept
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-extension"
    ::dispatch_apply_f(iterations, DISPATCH_APPLY_AUTO, ctx, function);
#pragma clang diagnostic pop
}

PSTLD_INTERNAL_IMPL void dispatch_async(void *ctx, void (*function)(void *)) noexcept
{
    ::dispatch_async_f(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ctx, function);
}

PSTLD_INTERNAL_IMPL DispatchGroup::DispatchGroup() noexcept
    : m_group(dispatch_group_create()), m_queue(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0))
{
}

PSTLD_INTERNAL_IMPL DispatchGroup::~DispatchGroup()
{
#if !defined(PSTLD_INTERNAL_ARC)
    ::dispatch_release(static_cast<dispatch_group_t>(m_group));
#endif
}

PSTLD_INTERNAL_IMPL void DispatchGroup::dispatch(void *ctx, void (*function)(void *)) noexcept
{
    ::dispatch_group_async_f(static_cast<dispatch_group_t>(m_group),
                             static_cast<dispatch_queue_t>(m_queue),
                             ctx,
                             function);
}

PSTLD_INTERNAL_IMPL void DispatchGroup::wait() noexcept
{
    ::dispatch_group_wait(static_cast<dispatch_group_t>(m_group), DISPATCH_TIME_FOREVER);
}

PSTLD_INTERNAL_IMPL const char *parallelism_exception::what() const noexcept
{
    return "Failed to acquire resources to perform parallel computation";
}

PSTLD_INTERNAL_IMPL void parallelism_exception::raise()
{
    throw parallelism_exception{};
};

} // namespace pstld::internal
