#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

bool distinct_test()
{
    int a1[]{ 1, 1, 2, 3, 3, 4, 5, 6 };
    int a2[]{ 1, 2, 3, 4, 5, 6 };
    auto e{ a1 >> distinct<int>() };
    return e >> equals(a2);
}

bool union_set_test()
{
    int a1[]{ 1, 1, 2, 3, 3, 4, 5, 6 };
    int a2[]{ 3, 4, 5, 6, 7, 7, 8 };
    int a3[]{ 1, 2, 3, 4, 5, 6, 7, 8 };
    auto e{ a1 >> union_set<int>(a2) };
    return e >> equals(a3);
}

bool intersect_test()
{
    int a1[]{ 1, 1, 2, 3, 3, 4, 5, 6 };
    int a2[]{ 3, 4, 5, 6, 7, 7, 8 };
    int a3[]{ 3, 4, 5, 6 };
    auto e{ a1 >> intersect<int>(a2) };
    return e >> equals(a3);
}

bool except_test()
{
    int a1[]{ 1, 1, 2, 3, 3, 4, 5, 6 };
    int a2[]{ 3, 4, 5, 6, 7, 7, 8 };
    int a3[]{ 1, 2 };
    auto e{ a1 >> except<int>(a2) };
    return e >> equals(a3);
}

int main()
{
    if (distinct_test() && union_set_test() && intersect_test() && except_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
