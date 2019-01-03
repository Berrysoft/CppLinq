#include "test_utility.hpp"
#include <algorithm>
#include <iostream>
#include <linq/query.hpp>
#include <linq/to_container.hpp>

using namespace std;
using namespace linq;

bool to_list_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    list<int> a2{ 2, 4, 6 };
    auto e = a1 >> where([](int& a) { return a % 2 == 0; }) >> to_list<int>();
    if (equal(e.begin(), e.end(), a2.begin(), a2.end()))
    {
        return true;
    }
    return false;
}

bool to_set_test()
{
    int a1[]{ 1, 1, 2, 3, 2, 4, 3, 5, 6, 2, 4, 3, 5, 2, 6, 1 };
    int a2[]{ 2, 4, 6 };
    auto s = a1 >> to_set<int>();
    auto e = s >> where([](int a) { return a % 2 == 0; });
    if (test::equal(e.begin(), e.end(), a2, a2 + 3))
    {
        return true;
    }
    return false;
}

bool to_vector_test()
{
    int a1[]{ 1, 2, 3, 4, 5, 6 };
    vector<int> a2{ 2, 4, 6 };
    auto e = a1 >> where([](int& a) { return a % 2 == 0; }) >> to_vector<int>();
    if (equal(e.begin(), e.end(), a2.begin(), a2.end()))
    {
        return true;
    }
    return false;
}

int main()
{
    if (to_list_test() && to_set_test() && to_vector_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
