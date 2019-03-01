#include "string_test.hpp"
#include <linq/string.hpp>

using namespace std;
using namespace linq;
using namespace bstest;

void string_test::split_test()
{
    string str{ "Hello world !" };
    string_view views[]{ "Hello", "world", "!" };
    auto e{ str >> split(' ') };
    test_equals(views, e);
}

void string_test::joinstr_test()
{
    {
        string str{ "123456" };
        int a1[]{ 1, 2, 3, 4, 5, 6 };
        auto s{ a1 >> joinstr<char>() };
        are_equal(str, s);
    }
    {
        string str{ "Hello world !" };
        string_view views[]{ "Hello", "world", "!" };
        auto s{ views >> joinstr<char>(' ') };
        are_equal(str, s);
    }
}

void string_test::instr_test()
{
    is_true("Hello world!" >> instr('o'));
    is_true("Hello world!" >> instr<char>("world"));
}

void string_test::replace_test()
{
    string str{ "Hello world!o" };
    string_view str2{ "Hellooo wooorld!ooo" };
    auto e{ str >> replace('o', "ooo") };
    are_equal(str2, e);
}

void string_test::remove_test()
{
    string str{ "Hello world!o" };
    string_view str2{ "Hell wrld!" };
    auto e{ str >> remove<char>("o") };
    are_equal(str2, e);
    auto e2{ str >> remove('o') };
    are_equal(str2, e2);
}

constexpr string_view test_str{ "123456" };

void string_test::starts_with_test()
{
    is_true(test_str >> starts_with('1'));
    is_true(test_str >> starts_with<char>("123"));
    is_false(test_str >> starts_with('2'));
    is_false(test_str >> starts_with<char>("456"));
}

void string_test::ends_with_test()
{
    is_true(test_str >> ends_with('6'));
    is_true(test_str >> ends_with<char>("456"));
    is_false(test_str >> ends_with('5'));
    is_false(test_str >> ends_with<char>("123"));
}

void string_test::trim_test()
{
    string str{ "   123456   " };
    auto tl{ str >> trim_left<char>() };
    are_equal("123456   ", tl);
    auto tr{ str >> trim_right<char>() };
    are_equal("   123456", tr);
    auto tall{ str >> trim<char>() };
    are_equal("123456", tall);
}

void string_test::io_lines_test()
{
    string str{ R"(Twinkle, twinkle, little star,
How I wonder what you are.
Up above the world so high,
Like a diamond in the sky.
)" };
    istringstream iss{ str };
    ostringstream oss;
    write_lines(oss, read_lines(iss));
    are_equal(str, oss.str());
}
