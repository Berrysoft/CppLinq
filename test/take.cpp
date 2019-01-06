#include <iostream>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

bool take_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 2, 3 };
    auto e{ a1 >> take(3) };
    return e >> equals(a2);
}

bool take_while_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 2, 3 };
    auto e{ a1 >> take_while([](int& i) { return i < 4; }) };
    return e >> equals(a2);
}

int main()
{
    if (take_test() && take_while_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
