#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    int a2[]{ 6, 5, 4, 3, 2, 1 };
    auto e{ a1 >> reverse<int>() };
    if (e >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
