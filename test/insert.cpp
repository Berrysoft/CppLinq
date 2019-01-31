#include <iostream>
#include <linq/core.hpp>
#include <vector>

using namespace std;
using namespace linq;

bool insert_item_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 2, 3, 0, 4, 5, 6 };
    auto e{ a1 >> insert(0, 3) };
    return e >> equals(a2);
}

bool insert_item_test2()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 2, 3, 4, 5, 6, 0 };
    auto e{ a1 >> insert(0, 6) };
    return e >> equals(a2);
}

bool insert_enumerable_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 2, 3, 0, 0, 0, 4, 5, 6 };
    auto e{ a1 >> insert(vector<int>{ 0, 0, 0 }, 3) };
    return e >> equals(a2);
}

bool insert_enumerable_test2()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 2, 3, 4, 5, 6, 0, 0, 0 };
    auto e{ a1 >> insert(vector<int>{ 0, 0, 0 }, 6) };
    return e >> equals(a2);
}

int main()
{
    if (insert_item_test() && insert_item_test2() && insert_enumerable_test() && insert_enumerable_test2())
    {
        cout << "Success." << endl;
    }
    return 0;
}
