#define BOOST_TEST_MODULE CoreTest

#include "test_utility.hpp"
#include <linq/core.hpp>
#include <linq/query.hpp>
#include <vector>

using namespace std;
using namespace linq;

BOOST_AUTO_TEST_CASE(enumerable_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    auto e1{ get_enumerable(a1) };
    auto e2{ get_enumerable(vector<int>{ 1, 2, 3, 4, 5, 6 }) };
    auto e3{ get_enumerable(begin(a1), end(a1)) };
    test_equals(e2, e1);
    test_equals(e3, e2);
}

BOOST_AUTO_TEST_CASE(construct_range_test)
{
    int a1[]{ 1, 2, 3, 4 };
    auto e1{ range(1, 5) };
    test_equals(a1, e1);

    int a2[]{ 2, 4, 6, 8 };
    auto e2{ range(2, 10, [](int i) { return i + 2; }) };
    test_equals(a2, e2);
}

BOOST_AUTO_TEST_CASE(construct_repeat_test)
{
    int a2[]{ 1, 1, 1, 1, 1 };
    auto e{ repeat(1, 5) };
    test_equals(a2, e);
}

BOOST_AUTO_TEST_CASE(calculate_concat_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 3, 5, 2, 4, 6 };
    auto e{ a1 >> where([](int& a) { return a % 2 != 0; }) >> concat(a1 >> where([](int& a) { return a % 2 == 0; })) };
    test_equals(a2, e);
}

BOOST_AUTO_TEST_CASE(calculate_pend_test)
{
    int a1[]{ 2, 3, 4, 5 };
    int a2[]{ 1, 2, 3, 4, 5, 6 };
    auto e{ a1 >> prepend(1) >> append(6) };
    test_equals(a2, e);
}

BOOST_AUTO_TEST_CASE(calculate_insert_item_test)
{
    {
        int a1[]{ 1, 2, 3, 4, 5, 6 };
        int a2[]{ 1, 2, 3, 0, 4, 5, 6 };
        auto e{ a1 >> insert(0, 3) };
        test_equals(a2, e);
    }
    {
        int a1[]{ 1, 2, 3, 4, 5, 6 };
        int a2[]{ 1, 2, 3, 4, 5, 6, 0 };
        auto e{ a1 >> insert(0, 6) };
        test_equals(a2, e);
    }
}

BOOST_AUTO_TEST_CASE(calculate_insert_enumerable_test)
{
    {
        int a1[]{ 1, 2, 3, 4, 5, 6 };
        int a2[]{ 1, 2, 3, 0, 0, 0, 4, 5, 6 };
        auto e{ a1 >> insert(vector<int>{ 0, 0, 0 }, 3) };
        test_equals(a2, e);
    }
    {
        int a1[]{ 1, 2, 3, 4, 5, 6 };
        int a2[]{ 1, 2, 3, 4, 5, 6, 0, 0, 0 };
        auto e{ a1 >> insert(vector<int>{ 0, 0, 0 }, 6) };
        test_equals(a2, e);
    }
}
