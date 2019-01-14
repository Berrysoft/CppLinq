#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 3, 4, 2, 6, 1, 5 };
    auto min{ a1 >> limit(less<void>{}) };
    auto max{ a1 >> limit(greater<void>{}) };
    if (min == 1 && max == 6)
    {
        cout << "Success." << endl;
    }
    return 0;
}
