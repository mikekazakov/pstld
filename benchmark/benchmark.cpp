// Copyright (c) 2021 Michael G. Kazakov. All rights reserved. Distributed under the MIT License.
#include <cstddef>
#include <cstdio>
#include <algorithm>
#include <chrono>
#include <execution>
#include <vector>
#include <array>
#include <iostream>
#include <random>
#include <cxxabi.h>

static constexpr size_t g_Iterations = 10;
static constexpr size_t g_IterationsDiscard = 1;
static const std::array<size_t, 7> g_Sizes =
    {100, 1000, 10'000, 100'000, 1'000'000, 10'000'000, 100'000'000};

template <class Tp>
inline void noopt(Tp const &value)
{
    asm volatile("" : : "r,m"(value) : "memory");
}

template <class Tp>
inline void noopt(Tp &value)
{
    asm volatile("" : "+r,m"(value) : : "memory");
}

template <class Setup, class Work>
std::chrono::steady_clock::duration measure(Setup setup, Work work)
{
    std::array<std::chrono::steady_clock::duration, g_Iterations> runs;
    for( size_t i = 0; i != g_Iterations; ++i ) {
        setup();
        const auto start = std::chrono::steady_clock::now();
        work();
        const auto end = std::chrono::steady_clock::now();
        runs[i] = end - start;
    }
    std::sort(runs.begin(), runs.end());
    return std::accumulate(runs.begin() + g_IterationsDiscard,
                           runs.end() - g_IterationsDiscard,
                           std::chrono::steady_clock::duration{});
}

namespace benchmarks {

template <class ExPo>
struct all_of { // 25.6.1
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure(
            [&] { v = std::vector<double>(size, 42.); },
            [&] {
                noopt(std::all_of(ExPo{}, v.begin(), v.end(), [](auto e) { return e < 50.; }));
            });
    }
};

template <class ExPo>
struct any_of { // 25.6.2
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure(
            [&] { v = std::vector<double>(size, 42.); },
            [&] {
                noopt(std::any_of(ExPo{}, v.begin(), v.end(), [](auto e) { return e > 50.; }));
            });
    }
};

template <class ExPo>
struct none_of { // 25.6.3
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure(
            [&] { v = std::vector<double>(size, 42.); },
            [&] {
                noopt(std::none_of(ExPo{}, v.begin(), v.end(), [](auto e) { return e > 50.; }));
            });
    }
};

template <class ExPo>
struct for_each { // 25.6.4
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure([&] { v = std::vector<double>(size, 42.); },
                       [&] {
                           std::for_each(ExPo{}, v.begin(), v.end(), [](auto &e) { e += 1.; });
                           noopt(v);
                       });
    }
};

template <class ExPo>
struct find { // 25.6.5
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure([&] { v = std::vector<double>(size, 42.); },
                       [&] { noopt(std::find(ExPo{}, v.begin(), v.end(), 50.)); });
    }
};

template <class ExPo>
struct find_end { // 25.6.6
    auto operator()(size_t size)
    {
        std::vector<double> v1, v2{42., 42., 42., 43.};
        return measure(
            [&] { v1 = std::vector<double>(size, 42.); },
            [&] { noopt(std::find_end(ExPo{}, v1.begin(), v1.end(), v2.begin(), v2.end())); });
    }
};

template <class ExPo>
struct find_first_of { // 25.6.7
    auto operator()(size_t size)
    {
        std::vector<double> v1, v2{43., 44., 45., 46.};
        return measure(
            [&] { v1 = std::vector<double>(size, 42.); },
            [&] { noopt(std::find_first_of(ExPo{}, v1.begin(), v1.end(), v2.begin(), v2.end())); });
    }
};

template <class ExPo>
struct adjacent_find { // 25.6.8
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure(
            [&] {
                v = std::vector<double>(size);
                std::iota(v.begin(), v.end(), 1.);
            },
            [&] { noopt(std::adjacent_find(ExPo{}, v.begin(), v.end())); });
    }
};

template <class ExPo>
struct count { // 25.6.9
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure([&] { v = std::vector<double>(size, 42.); },
                       [&] { noopt(std::count(ExPo{}, v.begin(), v.end(), 42.)); });
    }
};

template <class ExPo>
struct mismatch { // 25.6.10
    auto operator()(size_t size)
    {
        std::vector<double> v1, v2;
        return measure([&] { v1 = v2 = std::vector<double>(size, 42.); },
                       [&] { noopt(std::mismatch(ExPo{}, v1.begin(), v1.end(), v2.begin())); });
    }
};

template <class ExPo>
struct equal { // 25.6.11
    auto operator()(size_t size)
    {
        std::vector<double> v1, v2;
        return measure([&] { v1 = v2 = std::vector<double>(size, 42.); },
                       [&] { noopt(std::equal(ExPo{}, v1.begin(), v1.end(), v2.begin())); });
    }
};

template <class ExPo>
struct search { // 25.6.13
    auto operator()(size_t size)
    {
        std::vector<double> v1, v2{42., 42., 42., 43.};
        return measure(
            [&] { v1 = std::vector<double>(size, 42.); },
            [&] { noopt(std::search(ExPo{}, v1.begin(), v1.end(), v2.begin(), v2.end())); });
    }
};

template <class ExPo>
struct copy { // 25.7.1
    auto operator()(size_t size)
    {
        std::vector<double> v1, v2;
        return measure(
            [&] {
                v1 = std::vector<double>(size, 42.);
                v2 = std::vector<double>(size);
            },
            [&] {
                std::copy(ExPo{}, v1.begin(), v1.end(), v2.begin());
                noopt(v2);
            });
    }
};

template <class ExPo>
struct transform { // 25.7.4
    auto operator()(size_t size)
    {
        std::vector<double> v1, v2, v3;
        return measure(
            [&] {
                v1 = std::vector<double>(size, 42.);
                v2 = std::vector<double>(size, 71.);
                v3 = std::vector<double>(size);
            },
            [&] {
                std::transform(
                    ExPo{}, v1.begin(), v1.end(), v2.begin(), v3.begin(), [](auto a, auto b) {
                        return a * b;
                    });
                noopt(v3);
            });
    }
};

template <class ExPo>
struct replace { // 25.7.5
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure([&] { v = std::vector<double>(size, 42.); },
                       [&] {
                           std::replace(ExPo{}, v.begin(), v.end(), 42., 43.);
                           noopt(v);
                       });
    }
};

template <class ExPo>
struct fill { // 25.7.6
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure([&] { v = std::vector<double>(size); },
                       [&] {
                           std::fill(ExPo{}, v.begin(), v.end(), 42.);
                           noopt(v);
                       });
    }
};

template <class ExPo>
struct sort_Rnd { // 25.8.2.1, semi-random input
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure(
            [&] {
                std::mt19937 mt{42};
                std::uniform_real_distribution<double> dist{0., 1.};
                v = std::vector<double>(size);
                std::generate(std::begin(v), std::end(v), [&dist, &mt] { return dist(mt); });
            },
            [&] {
                std::sort(ExPo{}, v.begin(), v.end());
                noopt(v);
            });
    }
};

template <class ExPo>
struct sort_Eq { // 25.8.2.1, equal input
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure([&] { v = std::vector<double>(size, 42.); },
                       [&] {
                           std::sort(ExPo{}, v.begin(), v.end());
                           noopt(v);
                       });
    }
};

template <class ExPo>
struct sort_Asc { // 25.8.2.1, ascending
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure(
            [&] {
                v = std::vector<double>(size);
                std::iota(v.begin(), v.end(), 0.);
            },
            [&] {
                std::sort(ExPo{}, v.begin(), v.end());
                noopt(v);
            });
    }
};

template <class ExPo>
struct sort_Des { // 25.8.2.1, descending
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure(
            [&] {
                v = std::vector<double>(size);
                std::generate(
                    v.begin(), v.end(), [v = std::numeric_limits<double>::max()]() mutable {
                        return v -= 1.;
                    });
            },
            [&] {
                std::sort(ExPo{}, v.begin(), v.end());
                noopt(v);
            });
    }
};

template <class ExPo>
struct is_sorted { // 25.8.2.5
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure([&] { v = std::vector<double>(size, 42.); },
                       [&] { noopt(std::is_sorted(ExPo{}, v.begin(), v.end())); });
    }
};

template <class ExPo>
struct minmax_element { // 25.8.9
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure([&] { v = std::vector<double>(size, 42.); },
                       [&] { noopt(std::minmax_element(ExPo{}, v.begin(), v.end())); });
    }
};

template <class ExPo>
struct reduce { // 25.10.4
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure([&] { v = std::vector<double>(size, 42.); },
                       [&] { noopt(std::reduce(ExPo{}, v.begin(), v.end())); });
    }
};

template <class ExPo>
struct transform_reduce { // 25.10.6
    auto operator()(size_t size)
    {
        std::vector<double> v;
        return measure(
            [&] { v = std::vector<double>(size, 42.); },
            [&] {
                noopt(std::transform_reduce(
                    ExPo{}, v.begin(), v.end(), 0., std::plus<>{}, [](auto d) { return d + 1.; }));
            });
    }
};

template <class T>
static std::string demangle()
{
    const char *name = typeid(T).name();
    char s[1024];
    size_t len = sizeof(s);
    int status;
    std::string norm = abi::__cxa_demangle(name, s, &len, &status);
    norm.erase(0, std::string_view{"benchmarks::"}.length());
    norm.erase(norm.find_first_of('<'));
    return norm;
}

} // namespace benchmarks

struct Result {
    std::string name;
    std::array<double, g_Sizes.size()> speedups;
};

template <template <class> class Benchmark>
Result record()
{
    auto micro = [](auto d) {
        return std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(d).count();
    };
    using Seq = Benchmark<std::execution::sequenced_policy>;
    using Par = Benchmark<std::execution::parallel_policy>;
    Result r;
    r.name = benchmarks::demangle<Seq>();
    for( size_t i = 0; i != g_Sizes.size(); ++i )
        r.speedups[i] = micro(Seq{}(g_Sizes[i])) / micro(Par{}(g_Sizes[i]));
    return r;
}

int main()
{
    std::vector<Result> results;
    results.emplace_back(record<benchmarks::all_of>());
    results.emplace_back(record<benchmarks::any_of>());
    results.emplace_back(record<benchmarks::none_of>());
    results.emplace_back(record<benchmarks::for_each>());
    results.emplace_back(record<benchmarks::find>());
    results.emplace_back(record<benchmarks::find_end>());
    results.emplace_back(record<benchmarks::find_first_of>());
    results.emplace_back(record<benchmarks::adjacent_find>());
    results.emplace_back(record<benchmarks::count>());
    results.emplace_back(record<benchmarks::mismatch>());
    results.emplace_back(record<benchmarks::equal>());
    results.emplace_back(record<benchmarks::search>());
    results.emplace_back(record<benchmarks::copy>());
    results.emplace_back(record<benchmarks::transform>());
    results.emplace_back(record<benchmarks::replace>());
    results.emplace_back(record<benchmarks::fill>());
    results.emplace_back(record<benchmarks::sort_Rnd>());
    results.emplace_back(record<benchmarks::sort_Eq>());
    results.emplace_back(record<benchmarks::sort_Asc>());
    results.emplace_back(record<benchmarks::sort_Des>());
    results.emplace_back(record<benchmarks::is_sorted>());
    results.emplace_back(record<benchmarks::minmax_element>());
    results.emplace_back(record<benchmarks::reduce>());
    results.emplace_back(record<benchmarks::transform_reduce>());

    const auto max_name_len =
        std::max_element(results.begin(), results.end(), [](auto &a, auto &b) {
            return a.name.length() < b.name.length();
        })->name.length();

    printf("%*s", int(max_name_len + 1), "");
    for( auto s : g_Sizes ) {
        printf("%10lu ", s);
    }
    printf("\n");

    for( auto &r : results ) {
        printf("%-*s ", int(max_name_len), r.name.c_str());
        for( auto v : r.speedups )
            printf("%10.2f ", v);
        printf("\n");
    }
}
