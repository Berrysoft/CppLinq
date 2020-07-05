#define BOOST_TEST_MODULE QueryTest

#include "test_utility.hpp"
#include <cmath>
#include <linq/query.hpp>

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
    auto e{ a1 >> where([](int& a) { return a % 2 == 0; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(where_select_where_index_test)
{
    int a1[]{ 1, 1, 2, 4, 4, 5 };
    int a2[]{ 1, 2, 4, 5 };
    auto e{ a1 >> where_index([](int& a, size_t i) { return a == (int)i; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(where_select_select_test)
{
    int a1[]{ 1, 4, 9, 16, 25 };
    double a2[]{ 1, 2, 3, 4, 5 };
    auto e{ a1 >> select([](int& a) { return sqrt(a); }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(where_select_select_index_test)
{
    int a1[]{ 1, 4, 9, 16, 25 };
    pack a2[]{ { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 } };
    auto e{ a1 >> select_index([](int& a, size_t i) { return pack{ (int)i, (int)sqrt(a) }; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(where_select_select_many_test)
{
    pack a1[]{ { 1, 1 }, { 2, 4 }, { 3, 9 } };
    int a2[]{ 1, 1, 4, 16, 9, 81 };
    auto e{ a1 >> select_many([](pack& a) { return vector<int>{ a.arr[0], a.arr[1] }; }, [](pack&, int a) { return a * a; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

//BOOST_AUTO_TEST_CASE(where_select_select_many_index_test)
//{
//    pack a1[]{ { 1, 1 }, { 2, 4 }, { 3, 9 } };
//    int a2[]{ 0, 1, 1, 1, 4, 16, 4, 9, 81 };
//    auto e{ a1 >> select_many_index([](pack& a, size_t i) { return vector<int>{ (int)i, a.arr[0], a.arr[1] }; }, [](pack&, int a) { return a * a; }) };
//    test_equals(a2, e);
//}
//
//BOOST_AUTO_TEST_CASE(skip_take_skip_test)
//{
//    int a1[]{ 1, 2, 3, 4, 5, 6 };
//    int a2[]{ 4, 5, 6 };
//    auto e{ a1 >> skip(3) };
//    test_equals(a2, e);
//}
//
//BOOST_AUTO_TEST_CASE(skip_take_skip_while_test)
//{
//    int a1[]{ 1, 2, 3, 4, 5, 6 };
//    int a2[]{ 4, 5, 6 };
//    auto e{ a1 >> skip_while([](int& i) { return i < 4; }) };
//    test_equals(a2, e);
//}
//
//BOOST_AUTO_TEST_CASE(skip_take_skip_while_index_test)
//{
//    int a1[]{ 1, 2, 3, 4, 5, 6 };
//    int a2[]{ 4, 5, 6 };
//    auto e{ a1 >> skip_while_index([](int&, size_t i) { return i < 3; }) };
//    test_equals(a2, e);
//}
//
//BOOST_AUTO_TEST_CASE(skip_take_take_test)
//{
//    int a1[]{ 1, 2, 3, 4, 5, 6 };
//    int a2[]{ 1, 2, 3 };
//    auto e{ a1 >> take(3) };
//    test_equals(a2, e);
//}
//
//BOOST_AUTO_TEST_CASE(skip_take_take_while_test)
//{
//    int a1[]{ 1, 2, 3, 4, 5, 6 };
//    int a2[]{ 1, 2, 3 };
//    auto e{ a1 >> take_while([](int& i) { return i < 4; }) };
//    test_equals(a2, e);
//}
//
//BOOST_AUTO_TEST_CASE(skip_take_take_while_index_test)
//{
//    int a1[]{ 1, 2, 3, 4, 5, 6 };
//    int a2[]{ 1, 2, 3 };
//    auto e{ a1 >> take_while_index([](int&, size_t i) { return i < 3; }) };
//    test_equals(a2, e);
//}
//
//BOOST_AUTO_TEST_CASE(linear_zip_test)
//{
//    int a1[]{ 1, 2, 3, 4, 5, 6 };
//    int a2[]{ 101, 202, 303, 404, 505, 606 };
//    auto e{ a1 >> zip([](int i1, int i2) { return i1 + i2; }, vector<int>{ 100, 200, 300, 400, 500, 600 }) };
//    test_equals(a2, e);
//}
//
//BOOST_AUTO_TEST_CASE(linear_zip_index_test)
//{
//    int a1[]{ 1, 2, 3, 4, 5, 6 };
//    int a2[]{ 1, 4, 9, 16, 25, 36 };
//    auto e{ a1 >> zip_index([](int i1, int i2, size_t index) { return (int)(i1 + i2 * index); }, vector<int>{ 1, 2, 3, 4, 5, 6 }) };
//    test_equals(a2, e);
//}
//
//BOOST_AUTO_TEST_CASE(linear_cast_test)
//{
//    int a1[]{ 1, 4, 9, 16, 25 };
//    int a2[]{ 1, 2, 3, 4, 5 };
//    auto e{ a1 >> select([](int& a) { return sqrt(a); }) >> cast<int>() };
//    test_equals(a2, e);
//}
