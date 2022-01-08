#define PSTLD_HACK_INTO_STD
#include <pstld/pstld.h>
#include <Foundation/Foundation.h>
int main()
{
    NSString *v[] = {@"c", @"d", @"a", @"b", @"e"};
    auto cmp = [](NSString *first, NSString *second) { return [first compare:second] < 0; };
    std::sort(std::execution::par, std::begin(v), std::end(v), cmp);
    return !std::is_sorted(std::begin(v), std::end(v), cmp);
}
