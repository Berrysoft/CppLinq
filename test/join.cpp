#include <iostream>
#include <linq/aggregate.hpp>
#include <linq/string.hpp>
#include <string>

using namespace std;
using namespace linq;

bool join_test()
{
    string str{ "123456" };
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    auto s{ a1 >> join<char>() };
    return s == str;
}

bool join_with_char_test()
{
    string str{ "Hello world !" };
    string_view views[]{ "Hello", "world", "!" };
    auto s{ views >> join<char>(' ') };
    return s == str;
}

int main()
{
    if (join_test() && join_with_char_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
