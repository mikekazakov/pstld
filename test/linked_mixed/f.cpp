#define PSTLD_HACK_INTO_STD
#include <pstld/pstld.h>
bool f()
{
    int v[] = {5, 3, 7, 1, 5};
    std::sort(std::execution::par, std::begin(v), std::end(v));
    return std::is_sorted(std::begin(v), std::end(v));
}
