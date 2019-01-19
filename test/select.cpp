#include <cmath>
#include <iostream>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

bool select_test()
{
    int a1[]{ 1, 4, 9, 16, 25 };
    double a2[]{ 1, 2, 3, 4, 5 };
    auto e{ a1 >> select([](int& a) { return sqrt(a); }) };
    return e >> equals(a2);
}

bool select_index_test()
{
    int a1[]{ 1, 4, 9, 16, 25 };
    pair<size_t, double> a2[]{ { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 } };
    auto e{ a1 >> select_index([](int& a, size_t i) { return make_pair(i, sqrt(a)); }) };
    return e >> equals(a2);
}

int main()
{
    if (select_test() && select_index_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
