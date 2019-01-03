#include "test_utility.hpp"
#include <cmath>
#include <iostream>
#include <linq/query.hpp>
#include <vector>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3 };
    int a2[]{ 1, 1, 4, 16, 9, 81 };
    auto e{ a1 >> select_many([](int a) { return container_enumerable(vector<int>{ a, a * a }); }, [](int a) { return a * a; }) };
    if (test::equal(e, a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
