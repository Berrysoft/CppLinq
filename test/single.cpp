#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

bool single_test()
{
    int a1[]{ 1, 2, 3 };
    auto e{ a1 >> single([](int& i) { return i % 2 == 0; }) };
    return e == 2;
}

bool single_def_test()
{
    int a1[]{ 1, 3, 5 };
    auto e{ a1 >> single([](int& i) { return i % 2 == 0; }, 2) };
    return e == 2;
}

bool single_throw_test()
{
    int a1[]{ 2, 4, 6 };
    try
    {
        a1 >> single([](int& i) { return i % 2 == 0; });
    }
    catch (const more_than_one&)
    {
        return true;
    }
    return false;
}

int main()
{
    if (single_test() && single_def_test() && single_throw_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
