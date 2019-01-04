#include <iostream>
#include <linq/aggregate.hpp>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 101, 202, 303, 404, 505, 606 };
    auto e{ a1 >> zip(initializer_list<int>{ 100, 200, 300, 400, 500, 600 }, [](int i1, int i2) { return i1 + i2; }) };
    if (e >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
