#pragma once
#include "test_utility.hpp"

class enumerable_test : public bstest::test_base
{
public:
    enumerable_test() { add_test(test); }

private:
    void test();
};

class construct_test : public bstest::test_base
{
public:
    construct_test()
    {
        add_test(range_test);
        add_test(repeat_test);
    }

private:
    void range_test();
    void repeat_test();
};

class calculate_test : public bstest::test_base
{
public:
    calculate_test()
    {
        add_test(concat_test);
        add_test(pend_test);
        add_test(insert_item_test);
        add_test(insert_enumerable_test);
    }

private:
    void concat_test();
    void pend_test();
    void insert_item_test();
    void insert_enumerable_test();
};

class extension_test : public bstest::test_base
{
public:
    extension_test() { add_test(test); }

private:
    void test();
};
