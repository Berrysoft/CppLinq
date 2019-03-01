#include "winrt_test.hpp"

#ifdef LINQ_USE_WINRT

#include <winrt/Windows.Foundation.Collections.h>

#include <linq/winrt.hpp>

#include <linq/aggregate.hpp>
#include <linq/query.hpp>

using namespace std;
using namespace linq;
using namespace winrt;
using namespace bstest;

winrt_test::winrt_test()
{
    add_test(vector_test);
    add_test(map_test);
    init_apartment(apartment_type::single_threaded);
}

void winrt_test::vector_test()
{
    auto a1{ single_threaded_vector(vector<int>{ 1, 2, 3, 4, 5, 6 }) };
    int a2[]{ 2, 4, 6 };
    auto e{ a1 >> where([](int a) { return a % 2 == 0; }) };
    test_equals(a2, e);
}

void winrt_test::map_test()
{
    auto a1{ single_threaded_map(map<int, hstring>{ { 1, L"Gates" }, { 2, L"Jobs" }, { 3, L"Trump" } }) };
    auto a2{ single_threaded_map(map<int, int>{ { 2, 88 }, { 1, 92 }, { 3, 61 } }) };
    pack a3[]{ { L"Gates", 92 }, { L"Jobs", 88 }, { L"Trump", 61 } };
    auto e{ a1 >>
            join(a2,
                 [](auto a) { return a.Key(); },
                 [](auto a) { return a.Key(); },
                 [](auto a) { return a.Value(); },
                 [](auto a, auto e) { return pack{ a.Value(), e }; }) };
    test_equals(a3, e);
}

#endif
