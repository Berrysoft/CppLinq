#pragma once
#include "test_utility.hpp"

class where_select_test : public bstest::test_base
{
public:
    where_select_test()
    {
        add_test(where_test);
        add_test(where_index_test);
        add_test(select_test);
        add_test(select_index_test);
        add_test(select_many_test);
        add_test(select_many_index_test);
    }

private:
    void where_test();
    void where_index_test();
    void select_test();
    void select_index_test();
    void select_many_test();
    void select_many_index_test();

public:
    struct pack
    {
        int arr[2];
    };
};

inline std::ostream& operator<<(std::ostream& stream, const where_select_test::pack& p)
{
    return stream << '(' << p.arr[0] << ", " << p.arr[1] << ')';
}

inline bool operator==(const where_select_test::pack& p1, const where_select_test::pack& p2)
{
    return p1.arr[0] == p2.arr[0] && p1.arr[1] == p2.arr[1];
}
inline bool operator!=(const where_select_test::pack& p1, const where_select_test::pack& p2) { return !(p1 == p2); }

class skip_take_test : public bstest::test_base
{
public:
    skip_take_test()
    {
        add_test(skip_test);
        add_test(skip_while_test);
        add_test(skip_while_index_test);
        add_test(take_test);
        add_test(take_while_test);
        add_test(take_while_index_test);
    }

private:
    void skip_test();
    void skip_while_test();
    void skip_while_index_test();
    void take_test();
    void take_while_test();
    void take_while_index_test();
};

class linear_test : public bstest::test_base
{
public:
    linear_test()
    {
        add_test(zip_test);
        add_test(zip_index_test);
        add_test(cast_test);
    }

private:
    void zip_test();
    void zip_index_test();
    void cast_test();
};
