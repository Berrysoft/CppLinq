#include <ctime>
#include <iostream>
#include <linq/core.hpp>
#include <optional>
#include <random>
#include <vector>

using namespace std;
using namespace linq;

template <typename T>
class random_enumerator
{
private:
    vector<T> m_vec;
    optional<T> m_current;
    mt19937 rnd;

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

public:
    template <typename Eter>
    constexpr random_enumerator(Eter&& eter) : rnd((unsigned int)time(nullptr))
    {
        for (; eter; ++eter)
        {
            m_vec.emplace_back(*eter);
        }
        move_next();
    }

    constexpr operator bool() const { return (bool)m_current; }
    constexpr random_enumerator& operator++()
    {
        move_next();
        return *this;
    }
    constexpr decltype(auto) operator*() { return *m_current; }
};

inline auto as_random()
{
    return [](auto e) {
        using T = remove_reference_t<decltype(*e.begin())>;
        return enumerable(random_enumerator<T>(e.enumerator()));
    };
}

int main()
{
    auto e{ range(0, 10) >> as_random() };
    for (auto a : e)
    {
        cout << a << endl;
    }
    cout << "Success." << endl;
    return 0;
}
