#include "test_utility.hpp"
#include <iostream>
#include <linq/query.hpp>
#include <vector>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    auto e1{ get_enumerable(a1) };
    auto e2{ get_enumerable(vector<int>{ 1, 2, 3, 4, 5, 6 }) };
    if (test::equal(e1, e2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
