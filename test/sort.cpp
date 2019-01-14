#include <iostream>
#include <linq/aggregate.hpp>
#include <string>

using namespace std;
using namespace linq;

bool test_sort()
{
    int a1[]{ 6, 5, 4, 3, 2, 1 };
    int a2[]{ 1, 2, 3, 4, 5, 6 };
    auto e{ a1 >> sort() };
    return e >> equals(a2);
}

struct pack
{
    string name;
    int score;
};

inline bool operator==(const pack& p1, const pack& p2)
{
    return p1.name == p2.name && p1.score == p2.score;
}

inline bool operator!=(const pack& p1, const pack& p2)
{
    return !(p1 == p2);
}

bool test_sort_custom()
{
    pack a1[]{ { "Zh.F. Ren", 92 }, { "Jobs", 78 }, { "Trump", 85 }, { "Gates", 85 } };
    pack a2[]{ { "Zh.F. Ren", 92 }, { "Gates", 85 }, { "Trump", 85 }, { "Jobs", 78 } };
    auto e{ a1 >>
            sort(make_comparer([](pack& p) { return p.score; }, descending{}),
                 make_comparer([](pack& p) { return p.name; }, string_ascending<char>{})) };
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
