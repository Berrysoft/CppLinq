#include <winrt/Windows.Foundation.Collections.h>

#include <linq/winrt.hpp>

#include <iostream>
#include <linq/aggregate.hpp>
#include <linq/query.hpp>

using namespace std;
using namespace linq;
using namespace winrt;
using namespace Windows::Foundation::Collections;

bool vector_test()
{
    auto a1{ single_threaded_vector(vector<int>{ 1, 2, 3, 4, 5, 6 }) };
    int a2[]{ 2, 4, 6 };
    auto e{ a1 >> where([](int a) { return a % 2 == 0; }) };
    return e >> equals(a2);
}

struct pack
{
    hstring name;
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

bool map_test()
{
    auto a1{ single_threaded_map(map<int, hstring>{ { 1, L"Gates" }, { 2, L"Jobs" }, { 3, L"Trump" } }) };
    auto a2{ single_threaded_map(map<int, int>{ { 2, 88 }, { 1, 92 }, { 3, 61 } }) };
    pack a3[]{ { L"Gates", 92 }, { L"Jobs", 88 }, { L"Trump", 61 } };
    auto e{ a1 >>
            join<int, int>(
                a2,
                [](auto a) { return a.Key(); },
                [](auto a) { return a.Key(); },
                [](auto a) { return a.Value(); },
                [](auto a, auto e) { return pack{ a.Value(), e }; }) };
    return e >> equals(a3);
}

int main()
{
    init_apartment();
    if (vector_test() && map_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
