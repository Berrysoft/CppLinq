#include <cmath>
#include <iostream>
#include <linq/query.hpp>
#include <vector>

using namespace std;
using namespace linq;

struct pack
{
    int arr[2];
};

int main()
{
    pack a1[]{ { 1, 1 }, { 2, 4 }, { 3, 9 } };
    int a2[]{ 1, 1, 4, 16, 9, 81 };
    auto e{ a1 >> select_many([](pack& a) { return vector<int>{ a.arr[0], a.arr[1] }; }, [](int a) { return a * a; }) };
    if (e >> equals(a2))
    {
        cout << "Success." << endl;
    }
    return 0;
}
