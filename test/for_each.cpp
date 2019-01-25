#include <iostream>
#include <linq/aggregate.hpp>
#include <vector>

using namespace std;
using namespace linq;

bool for_each_test()
{
    int a1[]{ 1, 2, 3 };
    int a2[]{ 2, 4, 6 };
    a1 >> for_each([](int& i) { i *= 2; });
    return a1 >> equals(a2);
}

bool peek_test()
{
    int a1[]{ 1, 2, 3 };
    int a2[]{ 2, 4, 6 };
    auto e{ a1 >> peek([](int& i) { i *= 2; }) };
    return !(a1 >> equals(a2)) && e >> equals(a2);
}

bool for_each_index_test()
{
    int a1[]{ 1, 2, 3 };
    int a2[]{ 0, 2, 6 };
    a1 >> for_each_index([](int& i, size_t index) { i *= (int)index; });
    return a1 >> equals(a2);
}

bool peek_index_test()
{
    int a1[]{ 1, 2, 3 };
    int a2[]{ 0, 2, 6 };
    auto e{ a1 >> peek_index([](int& i, size_t index) { i *= (int)index; }) };
    return !(a1 >> equals(a2)) && e >> equals(a2);
}

int main()
{
    if (for_each_test() && peek_test() && for_each_index_test() && peek_index_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
