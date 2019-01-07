#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

bool test_sort()
{
    int a1[]{ 6, 5, 4, 3, 2, 1 };
    int a2[]{ 1, 2, 3, 4, 5, 6 };
    auto e{ a1 >> sort<int>() };
    return e >> equals(a2);
}

struct pack
{
    int index;
    int score;
};

constexpr bool operator==(const pack& p1, const pack& p2)
{
    return p1.index == p2.index && p1.score == p2.score;
}

constexpr bool operator!=(const pack& p1, const pack& p2)
{
    return !(p1 == p2);
}

bool test_sort_custom()
{
    pack a1[]{ { 1, 92 }, { 2, 78 }, { 3, 85 } };
    pack a2[]{ { 1, 92 }, { 3, 85 }, { 2, 78 } };
    auto e{ a1 >> sort<pack>([](pack& p) { return p.score; }, greater<int>{}) };
    return e >> equals(a2);
}

int main()
{
    if (test_sort() && test_sort_custom())
    {
        cout << "Success." << endl;
    }
    return 0;
}
