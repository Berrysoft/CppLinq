#include "test_utility.hpp"
#include <iostream>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

bool skip_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 4, 5, 6 };
    auto e{ a1 >> skip(3) };
    return test::equal(e, a2);
}

bool skip_while_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 4, 5, 6 };
    auto e{ a1 >> skip_while([](int& i) { return i < 4; }) };
    return test::equal(e, a2);
}

int main()
{
    if (skip_test() && skip_while_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
