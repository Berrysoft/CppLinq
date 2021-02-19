#define BOOST_TEST_MODULE QueryTest

#include "test_utility.hpp"
#include <cmath>
#include <linq/query.hpp>
#include <optional>

using namespace std;
using namespace linq;

struct pack
{
    int arr[2];
};

inline ostream& operator<<(ostream& stream, const pack& p)
{
    return stream << '(' << p.arr[0] << ", " << p.arr[1] << ')';
}

constexpr bool operator==(const pack& p1, const pack& p2) noexcept
{
    return p1.arr[0] == p2.arr[0] && p1.arr[1] == p2.arr[1];
}
constexpr bool operator!=(const pack& p1, const pack& p2) noexcept { return !(p1 == p2); }

BOOST_AUTO_TEST_CASE(where_select_where_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 2, 4, 6 };
    auto e{ a1 >> where([](int a) { return a % 2 == 0; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(where_select_test)
{
    int a1[]{ 1, 1, 2, 4, 4, 5 };
    int a2[]{ 1, 2, 4, 5 };
    auto e{ a1 >>
            with_index() >>
            where_select([](auto t) {
                auto [i, a] = t;
                return a == (int)i ? make_optional(a) : nullopt;
            }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(where_select_select_test)
{
    int a1[]{ 1, 4, 9, 16, 25 };
    double a2[]{ 1, 2, 3, 4, 5 };
    auto e{ a1 >> select([](int a) { return sqrt(a); }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(where_select_select_many_test)
{
    pack a1[]{ { 1, 1 }, { 2, 4 }, { 3, 9 } };
    int a2[]{ 1, 1, 4, 16, 9, 81 };
    auto e{ a1 >> select_many([](const pack& a) { return vector<int>{ a.arr[0], a.arr[1] }; }, [](const pack&, int a) { return a * a; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(skip_take_skip_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 4, 5, 6 };
    auto e{ a1 >> skip(3) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(skip_take_skip_while_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 4, 5, 6 };
    auto e{ a1 >> skip_while([](int i) { return i < 4; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(skip_take_take_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 2, 3 };
    auto e{ a1 >> take(3) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(skip_take_take_while_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 2, 3 };
    auto e{ a1 >> take_while([](int i) { return i < 4; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(linear_zip_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    vector<int> v{ 100, 200, 300, 400, 500, 600 };
    int a2[]{ 101, 202, 303, 404, 505, 606 };
    auto e{ a1 >> zip([](int i1, int i2) { return i1 + i2; }, v) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}
