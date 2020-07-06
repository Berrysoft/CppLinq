#define BOOST_TEST_MODULE StringTest

#include "test_utility.hpp"

#define NOMAXMIN
#define WIN32_LEAN_AND_MEAN

#include <winrt/Windows.Foundation.h>

#include <winrt/Windows.Foundation.Collections.h>

#include <linq/winrt.hpp>

#include <linq/aggregate.hpp>
#include <linq/query.hpp>

using namespace std;
using namespace linq;
using namespace winrt;

BOOST_AUTO_TEST_CASE(winrt_vector_test)
{
    auto a1{ single_threaded_vector(vector<int>{ 1, 2, 3, 4, 5, 6 }) };
    int a2[]{ 2, 4, 6 };
    auto e{ to_iterable(a1) >> where([](int a) { return a % 2 == 0; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

struct pack
{
    hstring name;
    int score;
};

inline ostream& operator<<(ostream& stream, const pack& p)
{
    return stream << '(' << to_string(p.name) << ", " << p.score << ')';
}

inline bool operator==(const pack& p1, const pack& p2)
{
    return p1.name == p2.name && p1.score == p2.score;
}
inline bool operator!=(const pack& p1, const pack& p2) { return !(p1 == p2); }

BOOST_AUTO_TEST_CASE(winrt_map_test)
{
    auto a1{ single_threaded_map(map<int, hstring>{ { 1, L"Gates" }, { 2, L"Jobs" }, { 3, L"Trump" } }) };
    auto a2{ single_threaded_map(map<int, int>{ { 2, 88 }, { 1, 92 }, { 3, 61 } }) };
    pack a3[]{ { L"Gates", 92 }, { L"Jobs", 88 }, { L"Trump", 61 } };
    auto e{ to_iterable(a1) >>
            join(
                to_iterable(a2),
                [](auto a) { return a.Key(); },
                [](auto a) { return a.Key(); },
                [](auto a) { return a.Value(); },
                [](auto a, auto e) { return pack{ a.Value(), e }; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a3, e);
}
