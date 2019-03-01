#pragma once
#include "test_utility.hpp"

class string_test : public bstest::test_base
{
public:
    string_test()
    {
        add_test(split_test);
        add_test(joinstr_test);
        add_test(instr_test);
        add_test(replace_test);
        add_test(remove_test);
        add_test(starts_with_test);
        add_test(ends_with_test);
        add_test(trim_test);
        add_test(io_lines_test);
    }

private:
    void split_test();
    void joinstr_test();
    void instr_test();
    void replace_test();
    void remove_test();
    void starts_with_test();
    void ends_with_test();
    void trim_test();
    void io_lines_test();
};
