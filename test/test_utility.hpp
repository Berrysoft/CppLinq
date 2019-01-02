#pragma once

namespace linq::test
{
    template <typename It1, typename It2>
    bool equal(It1 begin1, It1 end1, It2 begin2, It2 end2)
    {
        for (; begin1 != end1 && begin2 != end2; ++begin1, ++begin2)
        {
            if (*begin1 != *begin2)
                return false;
        }
        return begin1 == end1 && begin2 == end2;
    }
} // namespace linq::test
