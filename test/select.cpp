#include "test_utility.hpp"
#include <cmath>
#include <iostream>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 4, 9, 16, 25 };
    double a2[]{ 1, 2, 3, 4, 5 };
    auto e = enumerable(a1) >> select([](int& a) { return sqrt(a); });
    if (test::equal(e.begin(), e.end(), a2, a2 + 5))
    {
        cout << "Success." << endl;
    }
    return 0;
}
