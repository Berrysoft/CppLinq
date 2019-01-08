#include <iostream>
#include <linq/string.hpp>
#include <string>

using namespace std;
using namespace linq;

bool replace_test()
{
    string str{ "Hello world!" };
    string_view str2{ "Hellooo wooorld!" };
    auto e{ str >> replace<char>("o", "ooo") };
    return e == str2;
}

bool remove_test()
{
    string str{ "Hello world!" };
    string_view str2{ "Hell wrld!" };
    auto e{ str >> remove<char>("o") };
    return e == str2;
}

int main()
{
    if (replace_test() && remove_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
