#include <iostream>
#include <linq/string.hpp>
#include <string>

using namespace std;
using namespace linq;

bool trim_test()
{
    string str{ "   123456   " };
    auto tl{ str >> trim_left<char>() };
    auto tr{ str >> trim_right<char>() };
    auto tall{ str >> trim<char>() };
    return tl == "123456   " && tr == "   123456" && tall == "123456";
}

int main()
{
    if (trim_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
