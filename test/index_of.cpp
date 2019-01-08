#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    if ((a1 >> index_of([](int& a) { return a == 4; })) == 3)
    {
        cout << "Success." << endl;
    }
    return 0;
}
