#define BOOST_TEST_MODULE ExtensionTest

#include "test_utility.hpp"
#include <ctime>
#include <linq/core.hpp>
#include <optional>
#include <random>
#include <vector>

using namespace std;
using namespace linq;

constexpr auto as_random()
{
    return []<impl::container Container>(Container container)
               -> generator<typename std::iterator_traits<decltype(std::begin(container))>::value_type> {
        mt19937 rnd;
        vector<typename std::iterator_traits<decltype(std::begin(container))>::value_type> vec(std::begin(container), std::end(container));
        while (!vec.empty())
        {
            uniform_int_distribution<size_t> d{ 0, vec.size() - 1 };
            size_t index{ d(rnd) };
            co_yield vec[index];
            vec.erase(vec.begin() + index);
        }
    };
}

BOOST_AUTO_TEST_CASE(extension_test)
{
    auto e{ range(0, 10) >> as_random() };
    for (int i : e)
    {
        cout << i << ' ';
    }
    cout << endl;
    BOOST_CHECK(true);
}
