#include "test_utility.hpp"
#include <iostream>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 100, 200, 300, 400, 500, 600 };
    int a3[]{ 101, 202, 303, 404, 505, 606 };
    auto e = a1 >> zip(enumerable(a2), [](int i1, int i2) { return i1 + i2; });
    if (test::equal(e, a3))
    {
        cout << "Success." << endl;
    }
    return 0;
}
