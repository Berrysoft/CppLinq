#include <iostream>
#include <linq/query.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a2[]{ 1, 2, 3 };
    auto e{ range(1, 7) >> where([](int a) { return a % 2 == 0; }) >> select([](int a) { return a / 2; }) };
    if (e >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
