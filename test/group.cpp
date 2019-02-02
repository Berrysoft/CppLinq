#include <iostream>
#include <linq/aggregate.hpp>
#include <string>

using namespace std;
using namespace linq;

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

bool group_test()
{
    pack a1[]{ { 2, 88 }, { 1, 92 }, { 2, 78 }, { 1, 66 }, { 3, 85 }, { 3, 61 } };
    pack a2[]{ { 1, 79 }, { 2, 83 }, { 3, 73 } };
    auto e{ a1 >>
            group([](pack& a) { return a.index; },
                  [](pack& a) { return a.score; },
                  [](int key, auto e) { return pack{ key, e >> average() }; }) };
    return e >> equals(a2);
}

struct pack2
{
    int index;
    string name;
};

struct pack3
{
    string name;
    int score;
};

inline bool operator==(const pack3& p1, const pack3& p2)
{
    return p1.name == p2.name && p1.score == p2.score;
}

inline bool operator!=(const pack3& p1, const pack3& p2)
{
    return !(p1 == p2);
}

bool group_join_test()
{
    pack2 a1[]{ { 1, "Gates" }, { 2, "Jobs" }, { 3, "Trump" } };
    pack a2[]{ { 2, 88 }, { 1, 92 }, { 2, 78 }, { 1, 66 }, { 3, 85 }, { 3, 61 } };
    pack3 a3[]{ { "Gates", 79 }, { "Jobs", 83 }, { "Trump", 73 } };
    auto e{ a1 >>
            group_join(a2,
                       [](pack2& a) { return a.index; },
                       [](pack& a) { return a.index; },
                       [](pack& a) { return a.score; },
                       [](pack2& a, auto e) { return pack3{ a.name, e >> average() }; }) };
    return e >> equals(a3);
}

bool join_test()
{
    pack2 a1[]{ { 1, "Gates" }, { 2, "Jobs" }, { 3, "Trump" } };
    pack a2[]{ { 2, 88 }, { 1, 92 }, { 3, 61 } };
    pack3 a3[]{ { "Gates", 92 }, { "Jobs", 88 }, { "Trump", 61 } };
    auto e{ a1 >>
            join(a2,
                 [](pack2& a) { return a.index; },
                 [](pack& a) { return a.index; },
                 [](pack& a) { return a.score; },
                 [](pack2& a, auto e) { return pack3{ a.name, e }; }) };
    return e >> equals(a3);
}

int main()
{
    if (group_test() && group_join_test() && join_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
