#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 3, 4, 2, 6, 1, 5 };
    auto minimum{ a1 >> min() };
    auto maximum{ a1 >> max(7) };
    if (minimum == 1 && maximum == 7)
    {
        cout << "Success." << endl;
    }
    return 0;
}
