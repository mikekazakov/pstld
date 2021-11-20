// Copyright (c) 2021 Michael G. Kazakov. All rights reserved. Distributed under the MIT License.
#pragma once
#include <algorithm>
#include <numeric>
#include <iterator>
#include <vector>
#include <limits>
#include <mutex>
#include <cstddef>

namespace pstld {

namespace internal {

size_t max_hw_threads() noexcept;

void dispatch_apply(size_t iterations, void *ctx, void (*function)(void *, size_t)) noexcept;

template <class It>
using iterator_category_t = typename std::iterator_traits<It>::iterator_category;

template <class It>
inline constexpr bool is_random_iterator_v =
    std::is_convertible_v<iterator_category_t<It>, std::random_access_iterator_tag>;

template <class It>
inline constexpr bool is_bidirectional_iterator_v =
    std::is_convertible_v<iterator_category_t<It>, std::bidirectional_iterator_tag>;

template <class It>
using iterator_value_t = typename std::iterator_traits<It>::value_type;

template <class It>
using iterator_diff_t = typename std::iterator_traits<It>::difference_type;

template <class T>
inline constexpr bool can_be_atomic_v = std::conjunction_v<std::is_trivially_copyable<T>,
                                                           std::is_copy_constructible<T>,
                                                           std::is_move_constructible<T>,
                                                           std::is_copy_assignable<T>,
                                                           std::is_move_assignable<T>>;

struct no_op {
    template <typename T>
    T &&operator()(T &&v) const
    {
        return std::forward<T>(v);
    }
};

struct parallelism_exception : std::exception {
    const char *what() const noexcept override;
    [[noreturn]] static void raise();
};

template <class T>
struct parallelism_allocator {
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    T *allocate(size_t count)
    {
        if( void *ptr = ::operator new(count * sizeof(T), std::nothrow) )
            return static_cast<T *>(ptr);
        else
            parallelism_exception::raise();
    }

    void deallocate(T *ptr, size_t count) noexcept { ::operator delete(ptr, count * sizeof(T)); }

    template <class Other>
    bool operator==(const parallelism_allocator<Other> &) const noexcept
    {
        return true;
    }

    template <class Other>
    bool operator!=(const parallelism_allocator<Other> &) const noexcept
    {
        return false;
    }
};

template <class T>
using parallelism_vector = std::vector<T, parallelism_allocator<T>>;

template <class T>
struct unitialized_array : parallelism_allocator<T> {
    using allocator = parallelism_allocator<T>;
    T *m_data;
    size_t m_size;
    unitialized_array(size_t size) : m_size(size), m_data(allocator::allocate(size)) {}

    ~unitialized_array()
    {
        std::destroy(m_data, m_data + m_size);
        allocator::deallocate(m_data, m_size);
    }

    template <class... Args>
    void put(size_t ind, Args &&...vals) noexcept
    {
        ::new(m_data + ind) T(std::forward<Args>(vals)...);
    }

    T *begin() noexcept { return m_data; }

    T *end() noexcept { return m_data + m_size; }
};

inline constexpr size_t chunks_per_cpu = 8;

template <class T>
constexpr size_t work_chunks_min_fraction_1(T count)
{
    return std::min(max_hw_threads() * chunks_per_cpu, static_cast<size_t>(count));
}

template <class T>
constexpr size_t work_chunks_min_fraction_2(T count)
{
    return std::min(max_hw_threads() * chunks_per_cpu, static_cast<size_t>(count / 2));
}

template <class It>
struct ItRange {
    It first;
    It last;
};

template <class It, bool IsRandomAccess = is_random_iterator_v<It>>
struct Partition;

template <class It>
struct Partition<It, true> {
    It base;
    size_t fraction;
    size_t leftover;
    size_t count;
    Partition(It first, size_t count, size_t chunks)
        : base(first), fraction(count / chunks), leftover(count % chunks), count(count)
    {
    }

    ItRange<It> at(size_t chunk_no)
    {
        if( leftover ) {
            if( chunk_no >= leftover ) {
                const auto first =
                    base + (fraction + 1) * leftover + fraction * (chunk_no - leftover);
                const auto last = first + fraction;
                return {first, last};
            }
            else {
                const auto first = base + (fraction + 1) * chunk_no;
                const auto last = first + fraction + 1;
                return {first, last};
            }
        }
        else {
            const auto first = base + fraction * chunk_no;
            const auto last = first + fraction;
            return {first, last};
        }
    }
    
    It end()
    {
        return base + count;
    }
};

template <class It>
struct Partition<It, false> {
    parallelism_vector<ItRange<It>> segments;
    Partition(It first, size_t count, size_t chunks) : segments(chunks)
    {
        size_t fraction = count / chunks;
        size_t leftover = count % chunks;
        It it = first;
        for( size_t i = 0; i != chunks; ++i ) {
            auto first = it;
            auto diff = fraction;
            if( leftover != 0 ) {
                ++diff;
                --leftover;
            }
            auto last = std::next(it, diff);
            segments[i] = {first, last};
            it = last;
        }
    }

    ItRange<It> at(size_t chunk_no) { return segments[chunk_no]; }
    
    It end()
    {
        return segments.back().last;
    }
};

template <class It, bool = is_random_iterator_v<It> &&can_be_atomic_v<It>>
struct MinIteratorResult;

template <class It>
struct MinIteratorResult<It, true> {
    std::atomic<size_t> min_chunk;
    std::atomic<It> min;
    MinIteratorResult(It last) : min_chunk{std::numeric_limits<size_t>::max()}, min{last} {}

    void put(size_t chunk, It it)
    {
        It prev_it = min;
        while( prev_it > it && !min.compare_exchange_weak(prev_it, it) )
            ;

        size_t prev_chunk = min_chunk;
        while( prev_chunk > chunk && !min_chunk.compare_exchange_weak(prev_chunk, chunk) )
            ;
    }
};

template <class It>
struct MinIteratorResult<It, false> {
    std::atomic<size_t> min_chunk;
    It min;
    std::mutex mutex;

    MinIteratorResult(It last) : min_chunk{std::numeric_limits<size_t>::max()}, min{last} {}

    void put(size_t chunk, It it)
    {
        size_t prev = std::numeric_limits<size_t>::max();
        while( !min_chunk.compare_exchange_weak(prev, chunk) )
            if( prev < chunk )
                return;

        std::lock_guard lock{mutex};
        if( min_chunk == chunk )
            min = it;
    }
};

template <class It, bool = is_random_iterator_v<It> &&can_be_atomic_v<It>>
struct MaxIteratorResult;

template <class It>
struct MaxIteratorResult<It, true> {
    std::atomic<size_t> max_chunk;
    std::atomic<It> max;
    It last;

    MaxIteratorResult(It last)
        : max_chunk{std::numeric_limits<size_t>::max()}, max{last}, last(last)
    {
    }

    void put(size_t chunk, It it)
    {
        It prev_it = max;
        while( (prev_it == last || prev_it < it) && !max.compare_exchange_weak(prev_it, it) )
            ;

        size_t prev_chunk = max_chunk;
        while( static_cast<ptrdiff_t>(prev_chunk) < static_cast<ptrdiff_t>(chunk) &&
               !max_chunk.compare_exchange_weak(prev_chunk, chunk) )
            ;
    }
};

template <class It>
struct MaxIteratorResult<It, false> {
    std::atomic<size_t> max_chunk;
    It max;
    std::mutex mutex;

    MaxIteratorResult(It last) : max_chunk{std::numeric_limits<size_t>::max()}, max{last} {}

    void put(size_t chunk, It it)
    {
        size_t prev = std::numeric_limits<size_t>::max();
        while( !max_chunk.compare_exchange_weak(prev, chunk) )
            if( prev > chunk )
                return;

        std::lock_guard lock{mutex};
        if( max_chunk == chunk )
            max = it;
    }
};

template <class T>
struct Dispatchable {
    static void dispatch(void *ctx, size_t ind) noexcept { static_cast<T *>(ctx)->run(ind); }
    void dispatch_apply(size_t count) noexcept { internal::dispatch_apply(count, this, dispatch); }
};

} // namespace internal

namespace internal {

template <class It, class T, class BinOp, class UnOp>
struct TransformReduce : Dispatchable<TransformReduce<It, T, BinOp, UnOp>> {
    Partition<It> m_partition;
    unitialized_array<T> m_results;
    BinOp m_reduce;
    UnOp m_transform;

    TransformReduce(size_t count, size_t chunks, It first, BinOp reduce_op, UnOp transform_op)
        : m_partition(first, count, chunks), m_results(chunks), m_reduce(reduce_op),
          m_transform(transform_op)
    {
    }

    void run(size_t ind) noexcept
    {
        auto p = m_partition.at(ind);
        m_results.put(ind, transform_reduce_at_least_2(p.first, p.last));
    }

    T transform_reduce_at_least_2(It first, It last)
    {
        auto next = first;
        T val = m_reduce(m_transform(*first), m_transform(*++next));
        while( ++next != last )
            val = m_reduce(std::move(val), m_transform(*next));
        return val;
    }
};

} // namespace internal

template <class FwdIt, class T, class BinOp, class UnOp>
T transform_reduce(FwdIt first, FwdIt last, T val, BinOp reduce_op, UnOp transform_op) noexcept
{
    const auto count = std::distance(first, last);
    const auto chunks = internal::work_chunks_min_fraction_2(count);
    if( chunks > 1 ) {
        try {
            internal::TransformReduce<FwdIt, T, BinOp, UnOp> op{
                static_cast<size_t>(count), chunks, first, reduce_op, transform_op};
            op.dispatch_apply(chunks);
            return std::reduce(op.m_results.begin(), op.m_results.end(), val, reduce_op);
        } catch( const internal::parallelism_exception & ) {
        }
    }

    return std::transform_reduce(first, last, val, reduce_op, transform_op);
}

template <class It>
internal::iterator_value_t<It> reduce(It first, It last) noexcept
{
    using T = internal::iterator_value_t<It>;
    return ::pstld::transform_reduce(first, last, T{}, std::plus<>{}, ::pstld::internal::no_op{});
}

template <class It, class T>
T reduce(It first, It last, T val) noexcept
{
    return ::pstld::transform_reduce(first, last, val, std::plus<>{}, ::pstld::internal::no_op{});
}

template <class It, class T, class BinOp>
T reduce(It first, It last, T val, BinOp op) noexcept
{
    return ::pstld::transform_reduce(first, last, val, op, ::pstld::internal::no_op{});
}

namespace internal {

template <class It1, class It2, class T, class BinRedOp, class BinTrOp>
struct TransformReduce2 : Dispatchable<TransformReduce2<It1, It2, T, BinRedOp, BinTrOp>> {
    Partition<It1> m_partition1;
    Partition<It2> m_partition2;
    unitialized_array<T> m_results;
    BinRedOp m_reduce;
    BinTrOp m_transform;

    TransformReduce2(size_t count,
                     size_t chunks,
                     It1 first1,
                     It2 first2,
                     BinRedOp reduce_op,
                     BinTrOp transform_op)
        : m_partition1(first1, count, chunks), m_partition2(first2, count, chunks),
          m_results(chunks), m_reduce(reduce_op), m_transform(transform_op)
    {
    }

    void run(size_t ind) noexcept
    {
        auto p1 = m_partition1.at(ind);
        auto p2 = m_partition2.at(ind);
        m_results.put(ind, transform_reduce_at_least_2(p1.first, p1.last, p2.first));
    }

    T transform_reduce_at_least_2(It1 first1, It1 last1, It2 first2)
    {
        auto next1 = first1;
        auto next2 = first2;
        T val = m_reduce(m_transform(*first1, *first2), m_transform(*++next1, *++next2));
        while( ++next1 != last1 )
            val = m_reduce(std::move(val), m_transform(*next1, *++next2));
        return val;
    }
};

} // namespace internal

template <class FwdIt1, class FwdIt2, class T, class BinRedOp, class BinTrOp>
T transform_reduce(FwdIt1 first1,
                   FwdIt1 last1,
                   FwdIt2 first2,
                   T val,
                   BinRedOp reduce_op,
                   BinTrOp transform_op) noexcept
{
    const auto count = std::distance(first1, last1);
    const auto chunks = internal::work_chunks_min_fraction_2(count);
    if( chunks > 1 ) {
        try {
            internal::TransformReduce2<FwdIt1, FwdIt2, T, BinRedOp, BinTrOp> op{
                static_cast<size_t>(count), chunks, first1, first2, reduce_op, transform_op};
            op.dispatch_apply(chunks);
            return std::reduce(op.m_results.begin(), op.m_results.end(), val, reduce_op);
        } catch( const internal::parallelism_exception & ) {
        }
    }
    return std::transform_reduce(first1, last1, first2, val, reduce_op, transform_op);
}

template <class It1, class It2, class T>
T transform_reduce(It1 first1, It1 last1, It2 first2, T val) noexcept
{
    return ::pstld::transform_reduce(
        first1, last1, first2, val, std::plus<>{}, std::multiplies<>{});
}

namespace internal {

template <class It, class UnPred, bool Expected, bool Init>
struct AllOf : Dispatchable<AllOf<It, UnPred, Expected, Init>> {
    Partition<It> m_partition;
    UnPred m_pred;
    std::atomic_bool m_done{false};
    bool m_result = Init;

    AllOf(size_t count, size_t chunks, It first, UnPred pred)
        : m_partition(first, count, chunks), m_pred(pred)
    {
    }

    void run(size_t ind) noexcept
    {
        if( m_done )
            return;
        for( auto p = m_partition.at(ind); p.first != p.last; ++p.first )
            if( static_cast<bool>(m_pred(*p.first)) == !Expected ) {
                m_done = true;
                m_result = !Init;
                return;
            }
    }
};

} // namespace internal

template <class FwdIt, class UnPred>
bool all_of(FwdIt first, FwdIt last, UnPred pred) noexcept
{
    const auto count = std::distance(first, last);
    const auto chunks = internal::work_chunks_min_fraction_1(count);
    if( chunks > 1 ) {
        try {
            internal::AllOf<FwdIt, UnPred, true, true> op{
                static_cast<size_t>(count), chunks, first, pred};
            op.dispatch_apply(chunks);
            return op.m_result;
        } catch( const internal::parallelism_exception & ) {
        }
    }
    return std::all_of(first, last, pred);
}

template <class FwdIt, class UnPred>
bool none_of(FwdIt first, FwdIt last, UnPred pred) noexcept
{
    const auto count = std::distance(first, last);
    const auto chunks = internal::work_chunks_min_fraction_1(count);
    if( chunks > 1 ) {
        try {
            internal::AllOf<FwdIt, UnPred, false, true> op{
                static_cast<size_t>(count), chunks, first, pred};
            op.dispatch_apply(chunks);
            return op.m_result;
        } catch( const internal::parallelism_exception & ) {
        }
    }
    return std::none_of(first, last, pred);
}

template <class FwdIt, class UnPred>
bool any_of(FwdIt first, FwdIt last, UnPred pred) noexcept
{
    const auto count = std::distance(first, last);
    const auto chunks = internal::work_chunks_min_fraction_1(count);
    if( chunks > 1 ) {
        try {
            internal::AllOf<FwdIt, UnPred, false, false> op{
                static_cast<size_t>(count), chunks, first, pred};
            op.dispatch_apply(chunks);
            return op.m_result;
        } catch( const internal::parallelism_exception & ) {
        }
    }
    return std::any_of(first, last, pred);
}

namespace internal {

template <class It, class Func>
struct ForEach : Dispatchable<ForEach<It, Func>> {
    Partition<It> m_partition;
    Func m_func;

    ForEach(size_t count, size_t chunks, It first, Func func)
        : m_partition(first, count, chunks), m_func(func)
    {
    }

    void run(size_t ind) noexcept
    {
        for( auto p = m_partition.at(ind); p.first != p.last; ++p.first )
            m_func(*p.first);
    }
};

} // namespace internal

template <class FwdIt, class Func>
void for_each(FwdIt first, FwdIt last, Func func) noexcept
{
    const auto count = std::distance(first, last);
    const auto chunks = internal::work_chunks_min_fraction_1(count);
    if( chunks > 1 ) {
        try {
            internal::ForEach<FwdIt, Func> op{static_cast<size_t>(count), chunks, first, func};
            op.dispatch_apply(chunks);
            return;
        } catch( const internal::parallelism_exception & ) {
        }
    }
    std::for_each(first, last, func);
}

template <class FwdIt, class Size, class Func>
FwdIt for_each_n(FwdIt first, Size count, Func func) noexcept
{
    const auto chunks = internal::work_chunks_min_fraction_1(count);
    if( chunks > 1 ) {
        try {
            internal::ForEach<FwdIt, Func> op{static_cast<size_t>(count), chunks, first, func};
            op.dispatch_apply(chunks);
            return op.m_partition.end();
        } catch( const internal::parallelism_exception & ) {
        }
    }
    return std::for_each_n(first, count, func);
}

namespace internal {

template <class It, class Pred>
struct Count : Dispatchable<Count<It, Pred>> {
    Partition<It> m_partition;
    Pred m_pred;
    std::atomic<iterator_diff_t<It>> m_result{};

    Count(size_t count, size_t chunks, It first, Pred pred)
        : m_partition(first, count, chunks), m_pred(pred)
    {
    }

    void run(size_t ind) noexcept
    {
        auto p = m_partition.at(ind);
        m_result += std::count_if(p.first, p.last, m_pred);
    }
};

} // namespace internal

template <class FwdIt, class Pred>
typename std::iterator_traits<FwdIt>::difference_type
count_if(FwdIt first, FwdIt last, Pred pred) noexcept
{
    const auto count = std::distance(first, last);
    const auto chunks = internal::work_chunks_min_fraction_1(count);
    if( chunks > 1 ) {
        try {
            internal::Count<FwdIt, Pred> op{static_cast<size_t>(count), chunks, first, pred};
            op.dispatch_apply(chunks);
            return op.m_result;
        } catch( const internal::parallelism_exception & ) {
        }
    }
    return std::count_if(first, last, pred);
}

template <class FwdIt, class T>
typename std::iterator_traits<FwdIt>::difference_type
count(FwdIt first, FwdIt last, const T &value) noexcept
{
    return ::pstld::count_if(
        first, last, [&value](const auto &iter_value) { return iter_value == value; });
}

namespace internal {

template <class It, class Pred>
struct Find : Dispatchable<Find<It, Pred>> {
    Partition<It> m_partition;
    MinIteratorResult<It> m_result;
    Pred m_pred;

    Find(size_t count, size_t chunks, It first, It last, Pred pred)
        : m_partition(first, count, chunks), m_result{last}, m_pred(pred)
    {
    }

    void run(size_t ind) noexcept
    {
        if( ind < m_result.min_chunk ) {
            auto p = m_partition.at(ind);
            auto it = std::find_if(p.first, p.last, m_pred);
            if( it != p.last )
                m_result.put(ind, it);
        }
    }
};

} // namespace internal

template <class FwdIt, class Pred>
FwdIt find_if(FwdIt first, FwdIt last, Pred pred) noexcept
{
    const auto count = std::distance(first, last);
    const auto chunks = internal::work_chunks_min_fraction_1(count);
    if( chunks > 1 ) {
        try {
            internal::Find<FwdIt, Pred> op{static_cast<size_t>(count), chunks, first, last, pred};
            op.dispatch_apply(chunks);
            return op.m_result.min;
        } catch( const internal::parallelism_exception & ) {
        }
    }
    return std::find_if(first, last, pred);
}

template <class FwdIt, class T>
FwdIt find(FwdIt first, FwdIt last, const T &value) noexcept
{
    return ::pstld::find_if(
        first, last, [&value](const auto &iter_value) { return iter_value == value; });
}

template <class FwdIt, class Pred>
FwdIt find_if_not(FwdIt first, FwdIt last, Pred pred) noexcept
{
    return ::pstld::find_if(
        first, last, [&pred](const auto &value) { return !static_cast<bool>(pred(value)); });
}

template <class FwdIt1, class FwdIt2>
FwdIt1 find_first_of(FwdIt1 first1, FwdIt1 last1, FwdIt2 first2, FwdIt2 last2) noexcept
{
    return ::pstld::find_if(first1, last1, [first2, last2](const auto &value) {
        return std::find(first2, last2, value) != last2;
    });
}

template <class FwdIt1, class FwdIt2, class Pred>
FwdIt1 find_first_of(FwdIt1 first1, FwdIt1 last1, FwdIt2 first2, FwdIt2 last2, Pred pred) noexcept
{
    return ::pstld::find_if(first1, last1, [first2, last2, &pred](const auto &value1) {
        return std::find_if(first2, last2, [&value1, &pred](const auto &value2) {
                   return static_cast<bool>(pred(value1, value2));
               }) != last2;
    });
}

namespace internal {

template <class It, class Pred>
struct AdjacentFind : Dispatchable<AdjacentFind<It, Pred>> {
    Partition<It> m_partition;
    MinIteratorResult<It> m_result;
    Pred m_pred;

    AdjacentFind(size_t count, size_t chunks, It first, It last, Pred pred)
        : m_partition(first, count, chunks), m_result{last}, m_pred(pred)
    {
    }

    void run(size_t ind) noexcept
    {
        if( ind < m_result.min_chunk ) {
            auto p = m_partition.at(ind);
            for( auto it1 = p.first, it2 = p.first; it1 != p.last; it1 = it2 ) {
                ++it2;
                if( m_pred(*it1, *it2) ) {
                    m_result.put(ind, it1);
                    return;
                }
            }
        }
    }
};

} // namespace internal

template <class FwdIt, class Pred>
FwdIt adjacent_find(FwdIt first, FwdIt last, Pred pred) noexcept
{
    const auto count = std::distance(first, last);
    if( count > 1 ) {
        const auto chunks = internal::work_chunks_min_fraction_1(count - 1);
        if( chunks > 1 ) {
            try {
                internal::AdjacentFind<FwdIt, Pred> op{
                    static_cast<size_t>(count - 1), chunks, first, last, pred};
                op.dispatch_apply(chunks);
                return op.m_result.min;
            } catch( const internal::parallelism_exception & ) {
            }
        }
    }
    return std::adjacent_find(first, last, pred);
}

template <class FwdIt>
FwdIt adjacent_find(FwdIt first, FwdIt last) noexcept
{
    return ::pstld::adjacent_find(
        first, last, [](const auto &v1, const auto &v2) { return v1 == v2; });
}

namespace internal {

template <class It1, class It2, class Pred>
struct Search : Dispatchable<Search<It1, It2, Pred>> {
    Partition<It1> m_partition;
    MinIteratorResult<It1> m_result;
    Pred m_pred;
    It2 m_first2;
    It2 m_last2;

    Search(size_t count, size_t chunks, It1 first1, It1 last1, It2 first2, It2 last2, Pred pred)
        : m_partition(first1, count, chunks), m_result{last1}, m_pred(pred), m_first2(first2),
          m_last2(last2)
    {
    }

    void run(size_t ind) noexcept
    {
        if( ind < m_result.min_chunk ) {
            for( auto p = m_partition.at(ind); p.first != p.last; ++p.first ) {
                auto i1 = p.first;
                auto i2 = m_first2;
                for( ;; ++i1, ++i2 ) {
                    if( i2 == m_last2 ) {
                        m_result.put(ind, p.first);
                        return;
                    }
                    if( !m_pred(*i1, *i2) )
                        break;
                }
            }
        }
    }
};

} // namespace internal

template <class FwdIt1, class FwdIt2, class Pred>
FwdIt1 search(FwdIt1 first1, FwdIt1 last1, FwdIt2 first2, FwdIt2 last2, Pred pred) noexcept
{
    if( first1 == last1 || first2 == last2 )
        return first1;

    const auto count1 = std::distance(first1, last1);
    const auto count2 = std::distance(first2, last2);
    if( count1 < count2 )
        return last1;
    if( count1 == count2 )
        return std::equal(first1, last1, first2, last2, pred) ? first1 : last1;

    const auto count = count1 - count2 + 1;
    const auto chunks = internal::work_chunks_min_fraction_1(count);
    try {
        internal::Search<FwdIt1, FwdIt2, Pred> op{
            static_cast<size_t>(count), chunks, first1, last1, first2, last2, pred};
        op.dispatch_apply(chunks);
        return op.m_result.min;
    } catch( const internal::parallelism_exception & ) {
    }
    return std::search(first1, last1, first2, last2, pred);
}

template <class FwdIt1, class FwdIt2>
FwdIt1 search(FwdIt1 first1, FwdIt1 last1, FwdIt2 first2, FwdIt2 last2) noexcept
{
    return ::pstld::search(
        first1, last1, first2, last2, [](const auto &v1, const auto &v2) { return v1 == v2; });
}

namespace internal {

template <class It1, class It2, class Pred>
struct FindEnd : Dispatchable<FindEnd<It1, It2, Pred>> {
    Partition<It1> m_partition;
    MaxIteratorResult<It1> m_result;
    Pred m_pred;
    It2 m_first2;
    It2 m_last2;

    FindEnd(size_t count, size_t chunks, It1 first1, It1 last1, It2 first2, It2 last2, Pred pred)
        : m_partition(first1, count, chunks), m_result{last1}, m_pred(pred), m_first2(first2),
          m_last2(last2)
    {
    }

    void run(size_t ind) noexcept
    {
        if( static_cast<ptrdiff_t>(ind) < static_cast<ptrdiff_t>(m_result.max_chunk) )
            return;

        auto p = m_partition.at(ind);
        if constexpr( is_bidirectional_iterator_v<It1> ) {
            do {
                --p.last;
                auto i1 = p.last;
                auto i2 = m_first2;
                for( ;; ++i1, ++i2 ) {
                    if( i2 == m_last2 ) {
                        m_result.put(ind, p.last);
                        return;
                    }
                    if( !m_pred(*i1, *i2) )
                        break;
                }
            } while( p.first != p.last );
        }
        else {
            auto result = p.last;
            for( ; p.first != p.last; ++p.first ) {
                auto i1 = p.first;
                auto i2 = m_first2;
                for( ;; ++i1, ++i2 ) {
                    if( i2 == m_last2 ) {
                        result = p.first;
                        break;
                    }
                    if( !m_pred(*i1, *i2) )
                        break;
                }
            }
            if( result != p.last )
                m_result.put(ind, result);
        }
    }
};

} // namespace internal

template <class FwdIt1, class FwdIt2, class Pred>
FwdIt1 find_end(FwdIt1 first1, FwdIt1 last1, FwdIt2 first2, FwdIt2 last2, Pred pred) noexcept
{
    if( first1 == last1 )
        return first1;
    if( first2 == last2 )
        return last1;

    const auto count1 = std::distance(first1, last1);
    const auto count2 = std::distance(first2, last2);
    if( count1 < count2 )
        return last1;
    if( count1 == count2 )
        return std::equal(first1, last1, first2, last2, pred) ? first1 : last1;

    const auto count = count1 - count2 + 1;
    const auto chunks = internal::work_chunks_min_fraction_1(count);
    try {
        internal::FindEnd<FwdIt1, FwdIt2, Pred> op{
            static_cast<size_t>(count), chunks, first1, last1, first2, last2, pred};
        op.dispatch_apply(chunks);
        return op.m_result.max;
    } catch( const internal::parallelism_exception & ) {
    }
    return std::find_end(first1, last1, first2, last2, pred);
}

template <class FwdIt1, class FwdIt2>
FwdIt1 find_end(FwdIt1 first1, FwdIt1 last1, FwdIt2 first2, FwdIt2 last2) noexcept
{
    return ::pstld::find_end(
        first1, last1, first2, last2, [](const auto &v1, const auto &v2) { return v1 == v2; });
}

} // namespace pstld
