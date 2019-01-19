#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a2[]{ 1 };
    auto e{ get_enumerable<int*>(nullptr, nullptr) >> default_if_empty(1) };
    if (e >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
