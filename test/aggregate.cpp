#include "test_utility.hpp"
#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5 };
    auto i1{ a1 >> average() };
    auto i2{ a1 >> sum() };
    auto i3{ a1 >> aggregate(0, [](int& i1, int& i2) { return i1 + i2 * i2; }) };
    if (i1 == 3 && i2 == 15 && i3 == 55)
    {
        cout << "Success." << endl;
    }
    return 0;
}
