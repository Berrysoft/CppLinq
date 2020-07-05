#pragma once
#include <boost/test/unit_test.hpp>
#include <linq/core.hpp>

template <typename E1, typename E2>
void test_equals(E1&& e1, E2&& e2)
{
    auto eter1{ linq::get_enumerator(e1) };
    auto eter2{ linq::get_enumerator(e2) };
    for (; eter1 && eter2; ++eter1, ++eter2)
    {
        BOOST_CHECK_EQUAL(*eter1, *eter2);
    }
    BOOST_CHECK(!eter1 && !eter2);
}
template <typename E1, typename E2>
void test_noteqs(E1&& e1, E2&& e2)
{
    auto eter1{ linq::get_enumerator(e1) };
    auto eter2{ linq::get_enumerator(e2) };
    for (; eter1 && eter2; ++eter1, ++eter2)
    {
        if (*eter1 != *eter2)
            return;
    }
    BOOST_CHECK(eter1 || eter2);
}
