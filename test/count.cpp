#include "test_utility.hpp"
#include <iostream>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    auto c1 = enumerable(a1) >> count();
    auto c2 = enumerable(a1) >> count([](int& a) { return a % 2 == 0; });
    if (c1 == 6 && c2 == 3)
    {
        cout << "Success." << endl;
    }
    return 0;
}
