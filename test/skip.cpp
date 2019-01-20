#include <iostream>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

bool skip_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 4, 5, 6 };
    auto e{ a1 >> skip(3) };
    return e >> equals(a2);
}

bool skip_while_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 4, 5, 6 };
    auto e{ a1 >> skip_while([](int& i) { return i < 4; }) };
    return e >> equals(a2);
}

bool skip_while_index_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 4, 5, 6 };
    auto e{ a1 >> skip_while_index([](int&, size_t i) { return i < 3; }) };
    return e >> equals(a2);
}

int main()
{
    if (skip_test() && skip_while_test() && skip_while_index_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
