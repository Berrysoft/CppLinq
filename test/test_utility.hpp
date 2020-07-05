#pragma once
#include <boost/test/unit_test.hpp>

#define LINQ_CHECK_EQUAL_COLLECTIONS(c1, c2) BOOST_CHECK_EQUAL_COLLECTIONS(std::begin(c1), std::end(c1), std::begin(c2), std::end(c2))
