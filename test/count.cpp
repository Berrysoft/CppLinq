#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    auto c1{ a1 >> count() };
    auto c2{ a1 >> count([](int& a) { return a % 2 == 0; }) };
    if (c1 == 6 && c2 == 3)
    {
        cout << "Success." << endl;
    }
    return 0;
}
