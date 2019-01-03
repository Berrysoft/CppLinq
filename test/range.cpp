#include "test_utility.hpp"
#include <iostream>
#include <linq/core.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a2[]{ 1, 2, 3, 4 };
    auto e = range(1, 5);
    if (test::equal(e, a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}