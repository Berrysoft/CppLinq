#define BOOST_TEST_MODULE CoreTest

#include "test_utility.hpp"
#include <linq/core.hpp>
#include <linq/query.hpp>
#include <vector>

using namespace std;
using namespace linq;

BOOST_AUTO_TEST_CASE(construct_range_test)
{
    int a1[]{ 1, 2, 3, 4 };
    auto e1{ range(1, 5) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a1, e1);

    int a2[]{ 2, 4, 6, 8 };
    auto e2{ range(2, 10, [](int i) { return i + 2; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e2);
}

BOOST_AUTO_TEST_CASE(construct_repeat_test)
{
    int a2[]{ 1, 1, 1, 1, 1 };
    auto e{ repeat(1, 5) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(calculate_concat_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 3, 5, 2, 4, 6 };
    auto e{ a1 >> where([](int& a) { return a % 2 != 0; }) >> concat(a1 >> where([](int& a) { return a % 2 == 0; })) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(calculate_pend_test)
{
    int a1[]{ 2, 3, 4, 5 };
    int a2[]{ 1, 2, 3, 4, 5, 6 };
    auto e{ a1 >> prepend(1) >> append(6) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}
