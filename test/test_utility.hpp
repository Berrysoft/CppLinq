#pragma once
#include <iterator>

namespace linq::test
{
    template <typename C1, typename C2>
    bool equal(C1&& c1, C2&& c2)
    {
        auto begin1{ std::begin(c1) };
        auto end1{ std::end(c1) };
        auto begin2{ std::begin(c2) };
        auto end2{ std::end(c2) };
        for (; begin1 != end1 && begin2 != end2; ++begin1, ++begin2)
        {
            if (*begin1 != *begin2)
                return false;
        }
        return begin1 == end1 && begin2 == end2;
    }
} // namespace linq::test
