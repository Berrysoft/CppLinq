#include "test_utility.hpp"
#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 6, 5, 4, 3, 2, 1 };
    auto e{ a1 >> reverse() };
    if (test::equal(e, a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
