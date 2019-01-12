#include <iostream>
#include <linq/string.hpp>
#include <string>

using namespace std;
using namespace linq;

bool replace_test()
{
    string str{ "Hello world!o" };
    string_view str2{ "Hellooo wooorld!ooo" };
    auto e{ str >> replace<char>("o", "ooo") };
    return e == str2;
}

bool remove_test()
{
    string str{ "Hello world!o" };
    string_view str2{ "Hell wrld!" };
    auto e{ str >> remove<char>("o") };
    auto e2{ str >> remove('o') };
    return e == str2 && e2 == str2;
}

int main()
{
    if (replace_test() && remove_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
