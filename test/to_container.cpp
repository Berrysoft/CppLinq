#include <iostream>
#include <linq/aggregate.hpp>
#include <linq/query.hpp>
#include <linq/to_container.hpp>

using namespace std;
using namespace linq;

bool to_list_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    list<int> a2{ 2, 4, 6 };
    auto e{ a1 >> where([](int& a) { return a % 2 == 0; }) >> to_list<int>() };
    return e >> equals(a2);
}

bool to_set_test()
{
    int a1[]{ 1, 1, 2, 3, 2, 4, 3, 5, 6, 2, 4, 3, 5, 2, 6, 1 };
    int a2[]{ 2, 4, 6 };
    auto s{ a1 >> to_set<int>() };
    auto e{ s >> where([](int a) { return a % 2 == 0; }) };
    return e >> equals(a2);
}

bool to_vector_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    vector<int> a2{ 2, 4, 6 };
    auto e{ a1 >> where([](int& a) { return a % 2 == 0; }) >> to_vector<int>() };
    return e >> equals(a2);
}

bool to_map_test()
{
    int a1[]{ 1, 2, 3 };
    map<int, int> a2{
        { 1, 1 }, { 2, 4 }, { 3, 9 }
    };
    auto e{ a1 >> to_map([](int i) { return i; }, [](int i) { return i * i; }) };
    return e >> equals(a2);
}

int main()
{
    if (to_list_test() && to_set_test() && to_vector_test() && to_map_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
