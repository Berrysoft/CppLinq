#pragma once
#ifdef LINQ_USE_WINRT

#include "test_utility.hpp"

#define NOMAXMIN
#define WIN32_LEAN_AND_MEAN

#include <winrt/Windows.Foundation.h>

class winrt_test : public bstest::test_base
{
public:
    winrt_test();

private:
    void vector_test();
    void map_test();

public:
    struct pack
    {
        winrt::hstring name;
        int score;
    };
};

inline std::ostream& operator<<(std::ostream& stream, const winrt_test::pack& p)
{
    return stream << '(' << winrt::to_string(p.name) << ", " << p.score << ')';
}

inline bool operator==(const winrt_test::pack& p1, const winrt_test::pack& p2)
{
    return p1.name == p2.name && p1.score == p2.score;
}
inline bool operator!=(const winrt_test::pack& p1, const winrt_test::pack& p2) { return !(p1 == p2); }

#endif
