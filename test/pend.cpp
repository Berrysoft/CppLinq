#include <iostream>
#include <linq/core.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 2, 3, 4, 5 };
    int a2[]{ 1, 2, 3, 4, 5, 6 };
    auto e{ a1 >> prepend(1) >> append(6) };
    if (e >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
