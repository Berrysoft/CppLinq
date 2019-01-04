#include <cmath>
#include <iostream>
#include <linq/aggregate.hpp>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 4, 9, 16, 25 };
    int a2[]{ 1, 2, 3, 4, 5 };
    auto e{ a1 >> select([](int& a) { return sqrt(a); }) >> cast<int>() };
    if (e >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
