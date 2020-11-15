#define BOOST_TEST_MODULE StringTest

#include "test_utility.hpp"
#include <linq/string.hpp>

using namespace std;
using namespace linq;

BOOST_AUTO_TEST_CASE(string_split_test)
{
    string str{ "Hello world !" };
    string_view views[]{ "Hello", "world", "!" };
    auto e{ str >> split(' ') };
    LINQ_CHECK_EQUAL_COLLECTIONS(views, e);
}

BOOST_AUTO_TEST_CASE(string_joinstr_test)
{
    {
        string str{ "123456" };
        int a1[]{ 1, 2, 3, 4, 5, 6 };
        auto s{ a1 >> joinstr<char>() };
        BOOST_CHECK_EQUAL(str, s);
    }
    {
        string str{ "Hello world !" };
        string_view views[]{ "Hello", "world", "!" };
        auto s{ views >> joinstr<char>(' ') };
        BOOST_CHECK_EQUAL(str, s);
    }
}

BOOST_AUTO_TEST_CASE(string_instr_test)
{
    BOOST_CHECK("Hello world!" >> instr('o'));
    BOOST_CHECK("Hello world!" >> instr<char>("world"));
}

BOOST_AUTO_TEST_CASE(string_replace_test)
{
    string str{ "Hello world!o" };
    string_view str2{ "Hellooo wooorld!ooo" };
    auto e{ str >> replace('o', "ooo") };
    BOOST_CHECK_EQUAL(str2, e);
    auto e2{ e >> replace<char>("ooo", "o") };
    BOOST_CHECK_EQUAL(str, e2);
}

BOOST_AUTO_TEST_CASE(string_remove_test)
{
    string str{ "Hello world!o" };
    string_view str2{ "Hell wrld!" };
    auto e{ str >> remove<char>("o") };
    BOOST_CHECK_EQUAL(str2, e);
    auto e2{ str >> remove('o') };
    BOOST_CHECK_EQUAL(str2, e2);
}

constexpr string_view test_str{ "123456" };

BOOST_AUTO_TEST_CASE(string_starts_with_test)
{
    BOOST_CHECK(test_str >> starts_with('1'));
    BOOST_CHECK(test_str >> starts_with<char>("123"));
    BOOST_CHECK(!(test_str >> starts_with('2')));
    BOOST_CHECK(!(test_str >> starts_with<char>("456")));
}

BOOST_AUTO_TEST_CASE(string_ends_with_test)
{
    BOOST_CHECK(test_str >> ends_with('6'));
    BOOST_CHECK(test_str >> ends_with<char>("456"));
    BOOST_CHECK(!(test_str >> ends_with('5')));
    BOOST_CHECK(!(test_str >> ends_with<char>("123")));
}

BOOST_AUTO_TEST_CASE(string_trim_test)
{
    string str{ "   123456   " };
    auto tl{ str >> trim_left<char>() };
    BOOST_CHECK_EQUAL("123456   ", tl);
    auto tr{ str >> trim_right<char>() };
    BOOST_CHECK_EQUAL("   123456", tr);
    auto tall{ str >> trim<char>() };
    BOOST_CHECK_EQUAL("123456", tall);
}

BOOST_AUTO_TEST_CASE(string_io_lines_test)
{
    string str{ R"(Twinkle, twinkle, little star,
How I wonder what you are.
Up above the world so high,
Like a diamond in the sky.
)" };
    istringstream iss{ str };
    ostringstream oss;
    write_lines(oss, read_lines(iss));
    BOOST_CHECK_EQUAL(str, oss.str());
}
