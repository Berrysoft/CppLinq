#include <iostream>
#include <linq/aggregate.hpp>
#include <linq/core.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a2[]{ 1, 1, 1, 1, 1 };
    auto e{ repeat(1, 5) };
    if (e >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
