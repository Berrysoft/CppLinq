#define BOOST_TEST_MODULE ExtensionTest

#include "test_utility.hpp"
#include <ctime>
#include <linq/core.hpp>
#include <optional>
#include <random>
#include <vector>

using namespace std;
using namespace linq;

template <typename It>
class random_iterator_impl
{
private:
    using value_type = typename std::iterator_traits<It>::value_type;

    vector<value_type> m_vec;
    optional<value_type> m_current;
    mt19937 rnd;

public:
    using traits_type = impl::iterator_impl_traits<value_type>;

    random_iterator_impl(It begin, It end) : rnd((unsigned int)time(nullptr))
    {
        for (; begin != end; ++begin)
        {
            m_vec.emplace_back(*begin);
        }
        move_next();
    }

    typename traits_type::reference value() const { return *m_current; }

    void move_next()
    {
        if (m_vec.empty())
        {
            m_current = nullopt;
        }
        else
        {
            uniform_int_distribution<size_t> d{ 0, m_vec.size() - 1 };
            size_t index{ d(rnd) };
            m_current = m_vec[index];
            m_vec.erase(m_vec.begin() + index);
        }
    }

    bool is_valid() const { return (bool)m_current; }
};

template <typename It>
using random_iterator = impl::iterator_base<random_iterator_impl<It>>;

constexpr auto as_random()
{
    return [](auto&& container) {
        using It = decltype(std::begin(container));
        return impl::iterable{ random_iterator<It>{ impl::iterator_ctor, std::begin(container), std::end(container) } };
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
