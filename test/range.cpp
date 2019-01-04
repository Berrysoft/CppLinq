#include <iostream>
#include <linq/aggregate.hpp>
#include <linq/core.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a2[]{ 1, 2, 3, 4 };
    auto e{ range(1, 5) };
    if (e >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
