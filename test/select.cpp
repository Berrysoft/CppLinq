#include <cmath>
#include <iostream>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 4, 9, 16, 25 };
    double a2[]{ 1, 2, 3, 4, 5 };
    auto e{ a1 >> select([](int& a) { return sqrt(a); }) };
    if (e >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
