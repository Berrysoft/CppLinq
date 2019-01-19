#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 3, 4, 2, 6, 1, 5 };
    auto f1{ a1 >> first<int>() };
    auto f2{ a1 >> first<int>(0, [](int& i) { return i % 2 == 0; }) };
    auto l1{ a1 >> last<int>() };
    auto l2{ a1 >> last<int>(0, [](int& i) { return i % 2 == 0; }) };
    if (f1 == 3 && f2 == 4 && l1 == 5 && l2 == 6)
    {
        cout << "Success." << endl;
    }
    return 0;
}
