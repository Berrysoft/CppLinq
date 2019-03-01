#include "core_test.hpp"
#include <ctime>
#include <linq/core.hpp>
#include <linq/query.hpp>
#include <optional>
#include <random>
#include <vector>

using namespace std;
using namespace linq;
using namespace bstest;

void enumerable_test::test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    auto e1{ get_enumerable(a1) };
    auto e2{ get_enumerable(vector<int>{ 1, 2, 3, 4, 5, 6 }) };
    auto e3{ get_enumerable(begin(a1), end(a1)) };
    test_equals(e2, e1);
    test_equals(e3, e2);
}

void construct_test::range_test()
{
    int a1[]{ 1, 2, 3, 4 };
    auto e1{ range(1, 5) };
    test_equals(a1, e1);

    int a2[]{ 2, 4, 6, 8 };
    auto e2{ range(2, 10, [](int i) { return i + 2; }) };
    test_equals(a2, e2);
}

void construct_test::repeat_test()
{
    int a2[]{ 1, 1, 1, 1, 1 };
    auto e{ repeat(1, 5) };
    test_equals(a2, e);
}

void calculate_test::concat_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 3, 5, 2, 4, 6 };
    auto e{ a1 >> where([](int& a) { return a % 2 != 0; }) >> concat(a1 >> where([](int& a) { return a % 2 == 0; })) };
    test_equals(a2, e);
}

void calculate_test::pend_test()
{
    int a1[]{ 2, 3, 4, 5 };
    int a2[]{ 1, 2, 3, 4, 5, 6 };
    auto e{ a1 >> prepend(1) >> append(6) };
    test_equals(a2, e);
}

void calculate_test::insert_item_test()
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

void calculate_test::insert_enumerable_test()
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

template <typename T>
class random_enumerator
{
private:
    vector<T> m_vec;
    optional<T> m_current;
    mt19937 rnd;

    void move_next()
    {
        if (m_vec.empty())
        {
            m_current = nullopt;
        }
        else
        {
            uniform_int_distribution<size_t> d{ 0, m_vec.size() - 1 };
            size_t index{ d(rnd) };
            m_current = m_vec[index];
            m_vec.erase(m_vec.begin() + index);
        }
    }

public:
    template <typename Eter>
    constexpr random_enumerator(Eter&& eter) : rnd((unsigned int)time(nullptr))
    {
        for (; eter; ++eter)
        {
            m_vec.emplace_back(*eter);
        }
        move_next();
    }

    constexpr operator bool() const { return (bool)m_current; }
    constexpr random_enumerator& operator++()
    {
        move_next();
        return *this;
    }
    constexpr decltype(auto) operator*() { return *m_current; }
};

constexpr auto as_random()
{
    return [](auto e) {
        using T = remove_cref<decltype(*e.enumerator())>;
        return enumerable(random_enumerator<T>(e.enumerator()));
    };
}

void extension_test::test()
{
    auto e{ range(0, 10) >> as_random() };
    is_true(true);
}
