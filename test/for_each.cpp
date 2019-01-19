#include <iostream>
#include <linq/aggregate.hpp>
#include <vector>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    vector<int> a2;
    a1 >> for_each([&a2](int& i) { a2.push_back(i); });
    if (a1 >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
