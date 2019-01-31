#include <iostream>
#include <linq/query.hpp>
#include <vector>

using namespace std;
using namespace linq;

bool zip_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 101, 202, 303, 404, 505, 606 };
    auto e{ a1 >> zip([](int i1, int i2) { return i1 + i2; }, vector<int>{ 100, 200, 300, 400, 500, 600 }) };
    return e >> equals(a2);
}

bool zip_index_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 1, 4, 9, 16, 25, 36 };
    auto e{ a1 >> zip_index([](int i1, int i2, size_t index) { return i1 + i2 * index; }, vector<int>{ 1, 2, 3, 4, 5, 6 }) };
    return e >> equals(a2);
}

int main()
{
    if (zip_test() && zip_index_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
