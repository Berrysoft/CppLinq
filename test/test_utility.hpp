#pragma once
#include <bstest.hpp>
#include <linq/core.hpp>

namespace bstest
{
    template <typename E1, typename E2>
    void test_equals(E1&& e1, E2&& e2, const char* func, const char* file, std::size_t line)
    {
        try
        {
            auto eter1{ linq::get_enumerator(e1) };
            auto eter2{ linq::get_enumerator(e2) };
            for (; eter1 && eter2; ++eter1, ++eter2)
            {
                are_equal(*eter1, *eter2);
            }
        }
        catch (const assert_failed& e)
        {
            throw assert_failed{ sf::sprint("{}\nAt:\n{}", e.what(), impl::make_line_info(func, file, line)) };
        }
    }
    template <typename E1, typename E2>
    void test_noteqs(E1&& e1, E2&& e2, const char* func, const char* file, std::size_t line)
    {
        try
        {
            auto eter1{ linq::get_enumerator(e1) };
            auto eter2{ linq::get_enumerator(e2) };
            for (; eter1 && eter2; ++eter1, ++eter2)
            {
                are_noteq(*eter1, *eter2);
                if (*eter1 != *eter2)
                    break;
            }
        }
        catch (const assert_failed& e)
        {
            throw assert_failed{ sf::sprint("{}\nAt:\n{}", e.what(), impl::make_line_info(func, file, line)) };
        }
    }

#define test_equals(e1, e2) __call_assert_func(test_equals, e1, e2)
#define test_noteqs(e1, e2) __call_assert_func(test_noteqs, e1, e2)
} // namespace bstest
