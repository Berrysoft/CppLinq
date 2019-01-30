#include <iostream>
#include <linq/query.hpp>
#include <vector>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 101, 202, 303, 404, 505, 606 };
    auto e{ a1 >> zip([](int i1, int i2) { return i1 + i2; }, vector<int>{ 100, 200, 300, 400, 500, 600 }) };
    if (e >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
