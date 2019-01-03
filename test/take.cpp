#include "test_utility.hpp"
#include <iostream>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 2, 3 };
    auto e{ a1 >> take(3) };
    if (test::equal(e, a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
