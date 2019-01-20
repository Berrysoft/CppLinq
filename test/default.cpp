#include <iostream>
#include <linq/aggregate.hpp>

using namespace std;
using namespace linq;

int main()
{
    int a1[]{ 0 };
    int a2[]{ 1 };
    auto e1{ get_enumerable<int*>(nullptr, nullptr) >> default_if_empty() };
    auto e2{ get_enumerable<int*>(nullptr, nullptr) >> default_if_empty(1) };
    if (e1 >> equals(a1) && e2 >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
