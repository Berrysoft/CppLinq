#include <iostream>
#include <linq/string.hpp>
#include <string>

using namespace std;
using namespace linq;

constexpr string_view test_str{ "123456" };

bool starts_with_test()
{
    return test_str >> starts_with('1') &&
           !(test_str >> starts_with('2')) &&
           test_str >> starts_with<char>("123") &&
           !(test_str >> starts_with<char>("456"));
}

bool ends_with_test()
{
    return test_str >> ends_with('6') &&
           !(test_str >> ends_with('5')) &&
           test_str >> ends_with<char>("456") &&
           !(test_str >> ends_with<char>("123"));
}

int main()
{
    if (starts_with_test() && ends_with_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
