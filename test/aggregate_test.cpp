#define BOOST_TEST_MODULE AggregateTest

#include "test_utility.hpp"
#include <array>
#include <linq/aggregate.hpp>
#include <linq/query.hpp>
#include <linq/to_container.hpp>

using namespace std;
using namespace linq;

BOOST_AUTO_TEST_CASE(aggregate_count_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    auto c1{ a1 >> count() };
    BOOST_CHECK_EQUAL(6ULL, c1);
    auto c2{ a1 >> count([](int a) { return a % 2 == 0; }) };
    BOOST_CHECK_EQUAL(3ULL, c2);
}

BOOST_AUTO_TEST_CASE(aggregate_any_all_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    bool b1{ a1 >> any([](int a) { return a % 2 == 0; }) };
    bool b2{ a1 >> all([](int a) { return a < 7; }) };
    BOOST_CHECK(b1);
    BOOST_CHECK(b2);
}

BOOST_AUTO_TEST_CASE(aggregate_aggregate_method_test)
{
    int a1[]{ 1, 2, 3, 4, 5 };
    auto i1{ a1 >> average() };
    BOOST_CHECK_EQUAL(3, i1);
    auto i2{ a1 >> sum() };
    BOOST_CHECK_EQUAL(15, i2);
    auto i3{ a1 >> aggregate(0, [](int i1, int i2) { return i1 + i2 * i2; }) };
    BOOST_CHECK_EQUAL(55, i3);
}

BOOST_AUTO_TEST_CASE(aggregate_reverse_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 6, 5, 4, 3, 2, 1 };
    auto e1{ a1 >> reversed() };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e1);
    auto e2{ a1 >> reversed() >> reversed() };
    LINQ_CHECK_EQUAL_COLLECTIONS(a1, e2);
    set<int> a3{ 1, 2, 3, 4, 5, 6 };
    auto e3{ a3 >> reversed() };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e3);
}

BOOST_AUTO_TEST_CASE(aggregate_sort_test)
{
    int a1[]{ 6, 5, 4, 3, 2, 1 };
    int a2[]{ 1, 2, 3, 4, 5, 6 };
    auto e{ a1 >> sort() };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

struct aggregate_test_pack
{
    string name;
    int score;
};

inline ostream& operator<<(ostream& stream, const aggregate_test_pack& p)
{
    return stream << '(' << p.name << ", " << p.score << ')';
}

inline bool operator==(const aggregate_test_pack& p1, const aggregate_test_pack& p2)
{
    return p1.name == p2.name && p1.score == p2.score;
}
inline bool operator!=(const aggregate_test_pack& p1, const aggregate_test_pack& p2) { return !(p1 == p2); }

BOOST_AUTO_TEST_CASE(aggregate_sort_custom_test)
{
    aggregate_test_pack a1[]{ { "Zh.F. Ren", 92 }, { "Jobs", 78 }, { "Trump", 85 }, { "Gates", 85 } };
    aggregate_test_pack a2[]{ { "Zh.F. Ren", 92 }, { "Gates", 85 }, { "Trump", 85 }, { "Jobs", 78 } };
    auto e{ a1 >>
            sort(make_comparer([](const aggregate_test_pack& p) { return p.score; }, descending{}),
                 make_comparer([](const aggregate_test_pack& p) { return p.name; }, string_ascending<char>{})) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(aggregate_sorter_test)
{
    list<aggregate_test_pack> a1{ { "Zh.F. Ren", 92 }, { "Jobs", 78 }, { "Trump", 85 }, { "Gates", 85 } };
    aggregate_test_pack a2[]{ { "Zh.F. Ren", 92 }, { "Gates", 85 }, { "Trump", 85 }, { "Jobs", 78 } };
    a1.sort(make_sorter(make_comparer([](const aggregate_test_pack& p) { return p.score; }, descending{}),
                        make_comparer([](const aggregate_test_pack& p) { return p.name; }, string_ascending<char>{})));
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, a1);
}

BOOST_AUTO_TEST_CASE(aggregate_min_max_test)
{
    int a1[]{ 3, 4, 2, 6, 1, 5 };
    auto minimum{ a1 >> min() };
    BOOST_CHECK_EQUAL(1, minimum);
    auto maximum{ a1 >> max(7) };
    BOOST_CHECK_EQUAL(7, maximum);
}

BOOST_AUTO_TEST_CASE(aggregate_for_each_test)
{
    int a1[]{ 1, 2, 3 };
    int a2[]{ 2, 4, 6 };
    a1 >> for_each([](int& i) { i *= 2; });
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, a1);
}

BOOST_AUTO_TEST_CASE(aggregate_for_each_index_test)
{
    int a1[]{ 1, 2, 3 };
    int a2[]{ 0, 2, 6 };
    a1 >> for_each_index([](int& i, size_t index) { i *= (int)index; });
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, a1);
}

BOOST_AUTO_TEST_CASE(find_get_at_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    BOOST_CHECK_EQUAL(3, a1 >> get_at(2));
    BOOST_CHECK_EQUAL(1, a1 >> get_at(7, 1));
}

BOOST_AUTO_TEST_CASE(find_index_of_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    BOOST_CHECK_EQUAL(3ULL, a1 >> index_of([](int a) { return a == 4; }));
}

BOOST_AUTO_TEST_CASE(find_front_back_test)
{
    int a1[]{ 3, 4, 2, 6, 1, 5 };
    BOOST_CHECK_EQUAL(3, a1 >> front());
    BOOST_CHECK_EQUAL(4, a1 >> front([](int i) { return i % 2 == 0; }, 0));
    BOOST_CHECK_EQUAL(7, a1 >> front([](int i) { return i > 7; }, 7));
    BOOST_CHECK_EQUAL(5, a1 >> back());
    BOOST_CHECK_EQUAL(6, a1 >> back([](int i) { return i % 2 == 0; }, 0));
}

BOOST_AUTO_TEST_CASE(find_default_test)
{
    array<int, 0> empty{};
    int a1[]{ 0 };
    auto e1{ empty >> default_if_empty<int>() };
    LINQ_CHECK_EQUAL_COLLECTIONS(a1, e1);
    int a2[]{ 1 };
    auto e2{ empty >> default_if_empty(1) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e2);
}

BOOST_AUTO_TEST_CASE(find_single_test)
{
    {
        int a1[]{ 1, 2, 3 };
        auto e{ a1 >> single([](int i) { return i % 2 == 0; }) };
        BOOST_CHECK_EQUAL(2, e);
    }
    {
        int a1[]{ 1, 3, 5 };
        auto e{ a1 >> single([](int i) { return i % 2 == 0; }, 2) };
        BOOST_CHECK_EQUAL(2, e);
    }
    {
        int a1[]{ 2, 4, 6 };
        BOOST_CHECK_THROW(a1 >> single([](int i) { return i % 2 == 0; }), more_than_one);
    }
}

BOOST_AUTO_TEST_CASE(set_distinct_test)
{
    int a1[]{ 1, 1, 2, 3, 3, 4, 5, 6 };
    int a2[]{ 1, 2, 3, 4, 5, 6 };
    auto e{ a1 >> distinct() };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(set_union_set_test)
{
    int a1[]{ 1, 1, 2, 3, 3, 4, 5, 6 };
    int a2[]{ 3, 4, 5, 6, 7, 7, 8 };
    int a3[]{ 1, 2, 3, 4, 5, 6, 7, 8 };
    auto e{ a1 >> union_set(a2) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a3, e);
}

BOOST_AUTO_TEST_CASE(set_intersect_test)
{
    int a1[]{ 1, 1, 2, 3, 3, 4, 5, 6 };
    int a2[]{ 3, 4, 5, 6, 7, 7, 8 };
    int a3[]{ 3, 4, 5, 6 };
    auto e{ a1 >> intersect(a2) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a3, e);
}

BOOST_AUTO_TEST_CASE(set_except_test)
{
    int a1[]{ 1, 1, 2, 3, 3, 4, 5, 6 };
    int a2[]{ 3, 4, 5, 6, 7, 7, 8 };
    int a3[]{ 1, 2 };
    auto e{ a1 >> except(a2) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a3, e);
}

struct group_test_pack
{
    int index;
    int score;
};
struct group_test_pack2
{
    int index;
    string name;
};
struct group_test_pack3
{
    string name;
    int score;
};

inline ostream& operator<<(ostream& stream, const group_test_pack& p)
{
    return stream << '(' << p.index << ", " << p.score << ')';
}

inline bool operator==(const group_test_pack& p1, const group_test_pack& p2)
{
    return p1.index == p2.index && p1.score == p2.score;
}
inline bool operator!=(const group_test_pack& p1, const group_test_pack& p2) { return !(p1 == p2); }

inline ostream& operator<<(ostream& stream, const group_test_pack3& p)
{
    return stream << '(' << p.name << ", " << p.score << ')';
}

inline bool operator==(const group_test_pack3& p1, const group_test_pack3& p2)
{
    return p1.name == p2.name && p1.score == p2.score;
}
inline bool operator!=(const group_test_pack3& p1, const group_test_pack3& p2) { return !(p1 == p2); }

BOOST_AUTO_TEST_CASE(group_group_method_test)
{
    group_test_pack a1[]{ { 2, 88 }, { 1, 92 }, { 2, 78 }, { 1, 66 }, { 3, 85 }, { 3, 61 } };
    group_test_pack a2[]{ { 1, 79 }, { 2, 83 }, { 3, 73 } };
    auto e{ a1 >>
            group([](const group_test_pack& a) { return a.index; },
                  [](const group_test_pack& a) { return a.score; },
                  [](int key, auto e) { return group_test_pack{ key, e >> average() }; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(group_group_join_test)
{
    group_test_pack2 a1[]{ { 1, "Gates" }, { 2, "Jobs" }, { 3, "Trump" } };
    group_test_pack a2[]{ { 2, 88 }, { 1, 92 }, { 2, 78 }, { 1, 66 }, { 3, 85 }, { 3, 61 } };
    group_test_pack3 a3[]{ { "Gates", 79 }, { "Jobs", 83 }, { "Trump", 73 } };
    auto e{ a1 >>
            group_join(
                a2,
                [](const group_test_pack2& a) { return a.index; },
                [](const group_test_pack& a) { return a.index; },
                [](const group_test_pack& a) { return a.score; },
                [](const group_test_pack2& a, auto e) { return group_test_pack3{ a.name, e >> average() }; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a3, e);
}

BOOST_AUTO_TEST_CASE(group_join_test)
{
    group_test_pack2 a1[]{ { 1, "Gates" }, { 2, "Jobs" }, { 3, "Trump" } };
    group_test_pack a2[]{ { 2, 88 }, { 1, 92 }, { 3, 61 } };
    group_test_pack3 a3[]{ { "Gates", 92 }, { "Jobs", 88 }, { "Trump", 61 } };
    auto e{ a1 >>
            join(
                a2,
                [](const group_test_pack2& a) { return a.index; },
                [](const group_test_pack& a) { return a.index; },
                [](const group_test_pack& a) { return a.score; },
                [](const group_test_pack2& a, auto e) { return group_test_pack3{ a.name, e }; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a3, e);
}

BOOST_AUTO_TEST_CASE(to_container_to_list_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    list<int> a2{ 2, 4, 6 };
    auto e{ a1 >> where([](int& a) { return a % 2 == 0; }) >> to_list<int>() };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(to_container_to_set_test)
{
    int a1[]{ 1, 1, 2, 3, 2, 4, 3, 5, 6, 2, 4, 3, 5, 2, 6, 1 };
    int a2[]{ 2, 4, 6 };
    auto s{ a1 >> to_set<int>() };
    auto e{ s >> where([](int a) { return a % 2 == 0; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(to_container_to_multiset_test)
{
    int a1[]{ 1, 1, 2, 3, 2, 4, 3, 5, 6, 2, 4, 3, 5, 2, 6, 1 };
    int a2[]{ 2, 2, 2, 2, 4, 4, 6, 6 };
    auto s{ a1 >> to_multiset<int>() };
    auto e{ s >> where([](int a) { return a % 2 == 0; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(to_container_to_vector_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 2, 4, 6 };
    auto e{ a1 >> where([](int& a) { return a % 2 == 0; }) >> to_vector<int>() };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

BOOST_AUTO_TEST_CASE(to_container_to_deque_test)
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 2, 4, 6 };
    auto e{ a1 >> where([](int& a) { return a % 2 == 0; }) >> to_deque<int>() };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

namespace std
{
    ostream& operator<<(ostream& stream, const pair<const int, int>& p)
    {
        return stream << '(' << p.first << ", " << p.second << ')';
    }
} // namespace std

BOOST_AUTO_TEST_CASE(to_container_to_map_test)
{
    int a1[]{ 1, 2, 3 };
    map<int, int> a2{
        { 1, 1 }, { 2, 4 }, { 3, 9 }
    };
    auto e{ a1 >> to_map<int, int>([](int i) { return i; }, [](int i) { return i * i; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}

struct to_container_pack
{
    int index;
    int score;
};

BOOST_AUTO_TEST_CASE(to_container_to_multimap_test)
{
    to_container_pack a1[]{ { 1, 90 }, { 1, 78 }, { 3, 89 }, { 2, 68 }, { 2, 94 }, { 4, 79 } };
    multimap<int, int> a2{ { 1, 90 }, { 1, 78 }, { 2, 68 }, { 2, 94 }, { 3, 89 }, { 4, 79 } };
    auto e{ a1 >> to_multimap<int, int>([](to_container_pack& a) { return a.index; }, [](to_container_pack& a) { return a.score; }) };
    LINQ_CHECK_EQUAL_COLLECTIONS(a2, e);
}
