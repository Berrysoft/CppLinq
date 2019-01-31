#include <iostream>
#include <linq/core.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4 };
    int a2[]{ 2, 4, 6, 8 };
    auto e1{ range(1, 5) };
    auto e2{ range(2, 10, [](int i) { return i + 2; }) };
    if (e1 >> equals(a1) && e2 >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
