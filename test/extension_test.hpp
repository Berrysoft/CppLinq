#pragma once
#include "test_utility.hpp"

class extension_test : public bstest::test_base
{
public:
    extension_test() { add_test(test); }

private:
    void test();
};
