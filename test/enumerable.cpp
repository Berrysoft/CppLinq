#include <iostream>
#include <linq/query.hpp>
#include <vector>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    auto e1{ get_enumerable(a1) };
    auto e2{ get_enumerable(vector<int>{ 1, 2, 3, 4, 5, 6 }) };
    auto e3{ get_enumerable(begin(a1), end(a1)) };
    if (e1 >> equals(e2) && e2 >> equals(e3))
    {
        cout << "Success." << endl;
    }
    return 0;
}
