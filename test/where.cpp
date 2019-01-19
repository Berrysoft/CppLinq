#include <iostream>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

bool where_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 2, 4, 6 };
    auto e{ a1 >> where([](int& a) { return a % 2 == 0; }) };
    return e >> equals(a2);
}

bool where_index_test()
{
    int a1[]{ 1, 1, 2, 4, 4, 5 };
    int a2[]{ 1, 2, 4, 5 };
    auto e{ a1 >> where_index([](int& a, size_t i) { return a == i; }) };
    return e >> equals(a2);
}

int main()
{
    if (where_test() && where_index_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
