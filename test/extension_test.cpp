#define BOOST_TEST_MODULE ExtensionTest

#include "test_utility.hpp"
#include <chrono>
#include <linq/core.hpp>
#include <linq/string.hpp>
#include <optional>
#include <random>
#include <vector>

using namespace std;
using namespace linq;

struct as_random
{
    template <impl::container Container>
    auto operator()(Container container)
        -> generator<typename impl::container_traits<Container>::value_type>
    {
        mt19937 rnd{ (unsigned int)std::chrono::system_clock::now().time_since_epoch().count() };
        vector<typename std::iterator_traits<decltype(std::begin(container))>::value_type> vec(std::begin(container), std::end(container));
        while (!vec.empty())
        {
            uniform_int_distribution<size_t> d{ 0, vec.size() - 1 };
            size_t index{ d(rnd) };
            co_yield vec[index];
            vec.erase(vec.begin() + index);
        }
    }
};

BOOST_AUTO_TEST_CASE(extension_test)
{
    cout << (range(0, 10) >> as_random() >> joinstr<char>(' ')) << endl;
    BOOST_CHECK(true);
}
