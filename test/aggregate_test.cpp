#include "aggregate_test.hpp"
#include <linq/aggregate.hpp>
#include <linq/query.hpp>
#include <linq/to_container.hpp>

using namespace std;
using namespace linq;
using namespace bstest;

void aggregate_test::count_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    auto c1{ a1 >> count() };
    are_equal(6, c1);
    auto c2{ a1 >> count([](int& a) { return a % 2 == 0; }) };
    are_equal(3, c2);
}

void aggregate_test::any_all_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    bool b1{ a1 >> any([](int& a) { return a % 2 == 0; }) };
    bool b2{ a1 >> all([](int& a) { return a < 7; }) };
    is_true(b1);
    is_true(b2);
}

void aggregate_test::aggregate_method_test()
{
    int a1[]{ 1, 2, 3, 4, 5 };
    auto i1{ a1 >> average() };
    are_equal(3, i1);
    auto i2{ a1 >> sum() };
    are_equal(15, i2);
    auto i3{ a1 >> aggregate(0, [](int& i1, int& i2) { return i1 + i2 * i2; }) };
    are_equal(55, i3);
}

void aggregate_test::reverse_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 6, 5, 4, 3, 2, 1 };
    auto e{ a1 >> reverse() };
    test_equals(a2, e);
}

void aggregate_test::sort_test()
{
    int a1[]{ 6, 5, 4, 3, 2, 1 };
    int a2[]{ 1, 2, 3, 4, 5, 6 };
    auto e{ a1 >> sort() };
    test_equals(a2, e);
}

void aggregate_test::sort_custom_test()
{
    pack a1[]{ { "Zh.F. Ren", 92 }, { "Jobs", 78 }, { "Trump", 85 }, { "Gates", 85 } };
    pack a2[]{ { "Zh.F. Ren", 92 }, { "Gates", 85 }, { "Trump", 85 }, { "Jobs", 78 } };
    auto e{ a1 >>
            sort(make_comparer([](pack& p) { return p.score; }, descending{}),
                 make_comparer([](pack& p) { return p.name; }, string_ascending<char>{})) };
    test_equals(a2, e);
}

void aggregate_test::sorter_test()
{
    list<pack> a1{ { "Zh.F. Ren", 92 }, { "Jobs", 78 }, { "Trump", 85 }, { "Gates", 85 } };
    pack a2[]{ { "Zh.F. Ren", 92 }, { "Gates", 85 }, { "Trump", 85 }, { "Jobs", 78 } };
    a1.sort(make_sorter(make_comparer([](pack& p) { return p.score; }, descending{}),
                        make_comparer([](pack& p) { return p.name; }, string_ascending<char>{})));
    test_equals(a2, a1);
}

void aggregate_test::min_max_test()
{
    int a1[]{ 3, 4, 2, 6, 1, 5 };
    auto minimum{ a1 >> min() };
    are_equal(1, minimum);
    auto maximum{ a1 >> max(7) };
    are_equal(7, maximum);
}

void aggregate_test::for_each_test()
{
    int a1[]{ 1, 2, 3 };
    int a2[]{ 2, 4, 6 };
    a1 >> for_each([](int& i) { i *= 2; });
    test_equals(a2, a1);
}

void aggregate_test::for_each_index_test()
{
    int a1[]{ 1, 2, 3 };
    int a2[]{ 0, 2, 6 };
    a1 >> for_each_index([](int& i, size_t index) { i *= (int)index; });
    test_equals(a2, a1);
}

void aggregate_test::peek_test()
{
    int a1[]{ 1, 2, 3 };
    int a2[]{ 2, 4, 6 };
    auto e{ a1 >> peek([](int& i) { i *= 2; }) };
    test_noteqs(a2, a1);
    test_equals(a2, e);
}

void aggregate_test::peek_index_test()
{
    int a1[]{ 1, 2, 3 };
    int a2[]{ 0, 2, 6 };
    auto e{ a1 >> peek_index([](int& i, size_t index) { i *= (int)index; }) };
    test_noteqs(a2, a1);
    test_equals(a2, e);
}

void find_test::get_at_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    are_equal(3, a1 >> get_at(2));
    are_equal(1, get_enumerable<int*>(nullptr, nullptr) >> get_at(2, 1));
}

void find_test::index_of_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    are_equal(3, a1 >> index_of([](int& a) { return a == 4; }));
}

void find_test::front_back_test()
{
    int a1[]{ 3, 4, 2, 6, 1, 5 };
    are_equal(3, a1 >> front());
    are_equal(4, a1 >> front([](int& i) { return i % 2 == 0; }, 0));
    are_equal(7, a1 >> front([](int& i) { return i > 7; }, 7));
    are_equal(5, a1 >> back());
    are_equal(6, a1 >> back([](int& i) { return i % 2 == 0; }, 0));
}

void find_test::default_test()
{
    int a1[]{ 0 };
    auto e1{ get_enumerable<int*>(nullptr, nullptr) >> default_if_empty() };
    test_equals(a1, e1);
    int a2[]{ 1 };
    auto e2{ get_enumerable<int*>(nullptr, nullptr) >> default_if_empty(1) };
    test_equals(a2, e2);
}

void find_test::single_test()
{
    {
        int a1[]{ 1, 2, 3 };
        auto e{ a1 >> single([](int& i) { return i % 2 == 0; }) };
        are_equal(2, e);
    }
    {
        int a1[]{ 1, 3, 5 };
        auto e{ a1 >> single([](int& i) { return i % 2 == 0; }, 2) };
        are_equal(2, e);
    }
    {
        int a1[]{ 2, 4, 6 };
        throws_ex(const more_than_one&, [&a1]() { a1 >> single([](int& i) { return i % 2 == 0; }); }, [](const more_than_one&) { return true; });
    }
}

void set_test::distinct_test()
{
    int a1[]{ 1, 1, 2, 3, 3, 4, 5, 6 };
    int a2[]{ 1, 2, 3, 4, 5, 6 };
    auto e{ a1 >> distinct() };
    test_equals(a2, e);
}

void set_test::union_set_test()
{
    int a1[]{ 1, 1, 2, 3, 3, 4, 5, 6 };
    int a2[]{ 3, 4, 5, 6, 7, 7, 8 };
    int a3[]{ 1, 2, 3, 4, 5, 6, 7, 8 };
    auto e{ a1 >> union_set(a2) };
    test_equals(a3, e);
}

void set_test::intersect_test()
{
    int a1[]{ 1, 1, 2, 3, 3, 4, 5, 6 };
    int a2[]{ 3, 4, 5, 6, 7, 7, 8 };
    int a3[]{ 3, 4, 5, 6 };
    auto e{ a1 >> intersect(a2) };
    test_equals(a3, e);
}

void set_test::except_test()
{
    int a1[]{ 1, 1, 2, 3, 3, 4, 5, 6 };
    int a2[]{ 3, 4, 5, 6, 7, 7, 8 };
    int a3[]{ 1, 2 };
    auto e{ a1 >> except(a2) };
    test_equals(a3, e);
}

void group_test::group_method_test()
{
    pack a1[]{ { 2, 88 }, { 1, 92 }, { 2, 78 }, { 1, 66 }, { 3, 85 }, { 3, 61 } };
    pack a2[]{ { 1, 79 }, { 2, 83 }, { 3, 73 } };
    auto e{ a1 >>
            group([](pack& a) { return a.index; },
                  [](pack& a) { return a.score; },
                  [](int key, auto e) { return pack{ key, e >> average() }; }) };
    test_equals(a2, e);
}

void group_test::group_join_test()
{
    pack2 a1[]{ { 1, "Gates" }, { 2, "Jobs" }, { 3, "Trump" } };
    pack a2[]{ { 2, 88 }, { 1, 92 }, { 2, 78 }, { 1, 66 }, { 3, 85 }, { 3, 61 } };
    pack3 a3[]{ { "Gates", 79 }, { "Jobs", 83 }, { "Trump", 73 } };
    auto e{ a1 >>
            group_join(a2,
                       [](pack2& a) { return a.index; },
                       [](pack& a) { return a.index; },
                       [](pack& a) { return a.score; },
                       [](pack2& a, auto e) { return pack3{ a.name, e >> average() }; }) };
    test_equals(a3, e);
}

void group_test::join_test()
{
    pack2 a1[]{ { 1, "Gates" }, { 2, "Jobs" }, { 3, "Trump" } };
    pack a2[]{ { 2, 88 }, { 1, 92 }, { 3, 61 } };
    pack3 a3[]{ { "Gates", 92 }, { "Jobs", 88 }, { "Trump", 61 } };
    auto e{ a1 >>
            join(a2,
                 [](pack2& a) { return a.index; },
                 [](pack& a) { return a.index; },
                 [](pack& a) { return a.score; },
                 [](pack2& a, auto e) { return pack3{ a.name, e }; }) };
    test_equals(a3, e);
}

void to_container_test::to_list_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    list<int> a2{ 2, 4, 6 };
    auto e{ a1 >> where([](int& a) { return a % 2 == 0; }) >> to_list<int>() };
    test_equals(a2, e);
}

void to_container_test::to_set_test()
{
    int a1[]{ 1, 1, 2, 3, 2, 4, 3, 5, 6, 2, 4, 3, 5, 2, 6, 1 };
    int a2[]{ 2, 4, 6 };
    auto s{ a1 >> to_set<int>() };
    auto e{ s >> where([](int a) { return a % 2 == 0; }) };
    test_equals(a2, e);
}

void to_container_test::to_multiset_test()
{
    int a1[]{ 1, 1, 2, 3, 2, 4, 3, 5, 6, 2, 4, 3, 5, 2, 6, 1 };
    int a2[]{ 2, 2, 2, 2, 4, 4, 6, 6 };
    auto s{ a1 >> to_multiset<int>() };
    auto e{ s >> where([](int a) { return a % 2 == 0; }) };
    test_equals(a2, e);
}

void to_container_test::to_vector_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    vector<int> a2{ 2, 4, 6 };
    auto e{ a1 >> where([](int& a) { return a % 2 == 0; }) >> to_vector<int>() };
    test_equals(a2, e);
}

void to_container_test::to_map_test()
{
    int a1[]{ 1, 2, 3 };
    map<int, int> a2{
        { 1, 1 }, { 2, 4 }, { 3, 9 }
    };
    auto e{ a1 >> to_map<int, int>([](int i) { return i; }, [](int i) { return i * i; }) };
    is_true(e >> equals(a2));
}

void to_container_test::to_multimap_test()
{
    pack a1[]{ { 1, 90 }, { 1, 78 }, { 3, 89 }, { 2, 68 }, { 2, 94 }, { 4, 79 } };
    multimap<int, int> a2{ { 1, 90 }, { 1, 78 }, { 2, 68 }, { 2, 94 }, { 3, 89 }, { 4, 79 } };
    auto e{ a1 >> to_multimap<int, int>([](pack& a) { return a.index; }, [](pack& a) { return a.score; }) };
    is_true(e >> equals(a2));
}
