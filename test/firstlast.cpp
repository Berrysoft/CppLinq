#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 3, 4, 2, 6, 1, 5 };
    auto f1{ a1 >> front() };
    auto f2{ a1 >> front([](int& i) { return i % 2 == 0; }, 0) };
    auto f3{ a1 >> front([](int& i) { return i > 7; }, 7) };
    auto l1{ a1 >> back() };
    auto l2{ a1 >> back([](int& i) { return i % 2 == 0; }, 0) };
    if (f1 == 3 && f2 == 4 && f3 == 7 && l1 == 5 && l2 == 6)
    {
        cout << "Success." << endl;
    }
    return 0;
}
