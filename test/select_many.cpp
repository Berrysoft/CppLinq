#include <cmath>
#include <iostream>
#include <linq/query.hpp>
#include <vector>

using namespace std;
using namespace linq;

struct pack
{
    int arr[2];
};

bool select_many_test()
{
    pack a1[]{ { 1, 1 }, { 2, 4 }, { 3, 9 } };
    int a2[]{ 1, 1, 4, 16, 9, 81 };
    auto e{ a1 >> select_many([](pack& a) { return vector<int>{ a.arr[0], a.arr[1] }; }, [](pack&, int a) { return a * a; }) };
    return e >> equals(a2);
}

bool select_many_index_test()
{
    pack a1[]{ { 1, 1 }, { 2, 4 }, { 3, 9 } };
    int a2[]{ 0, 1, 1, 1, 4, 16, 4, 9, 81 };
    auto e{ a1 >> select_many_index([](pack& a, size_t i) { return vector<int>{ (int)i, a.arr[0], a.arr[1] }; }, [](pack&, int a) { return a * a; }) };
    return e >> equals(a2);
}

int main()
{
    if (select_many_test() && select_many_index_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
