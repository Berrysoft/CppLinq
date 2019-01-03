#include "test_utility.hpp"
#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    bool b1{ a1 >> any([](int& a) { return a % 2 == 0; }) };
    bool b2{ a1 >> all([](int& a) { return a < 7; }) };
    if (b1 && b2)
    {
        cout << "Success." << endl;
    }
    return 0;
}
