#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    if ((a1 >> get_at(2)) == 3 && ((get_enumerable<int*>(nullptr, nullptr) >> get_at(2, 1)) == 1))
    {
        cout << "Success." << endl;
    }
    return 0;
}
