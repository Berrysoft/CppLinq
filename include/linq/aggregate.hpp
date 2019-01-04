#ifndef LINQ_AGGREGATE_HPP
#define LINQ_AGGREGATE_HPP

#include <algorithm>
#include <linq/core.hpp>
#include <vector>

namespace linq
{
    namespace impl
    {
        constexpr auto allways_true()
        {
            return [](auto) { return true; };
        }
    } // namespace impl

    template <typename Pred = decltype(impl::allways_true())>
    constexpr auto count(Pred&& pred = impl::allways_true())
    {
        return [&](auto e) {
            std::size_t result{ 0 };
            for (auto item : e)
            {
                if (pred(item))
                    ++result;
            }
            return result;
        };
    }

    template <typename Pred>
    constexpr auto all(Pred&& pred)
    {
        return [&](auto e) {
            for (auto item : e)
            {
                if (!pred(item))
                    return false;
            }
            return true;
        };
    }

    template <typename Pred = decltype(impl::allways_true())>
    constexpr auto any(Pred&& pred = impl::allways_true())
    {
        return [&](auto e) {
            for (auto item : e)
            {
                if (pred(item))
                    return true;
            }
            return false;
        };
    }

    template <typename T, typename Func>
    constexpr auto aggregate(T&& seed, Func&& func)
    {
        return [&](auto e) {
            T result{ seed };
            for (auto item : e)
            {
                result = func(result, item);
            }
            return result;
        };
    }

    constexpr auto average()
    {
        return [](auto e) {
            using T = std::remove_const_t<std::remove_reference_t<decltype(*e.enumerator())>>;
            T sum{ 0 };
            std::size_t num{ 0 };
            for (auto item : e)
            {
                sum += item;
                ++num;
            }
            return (T)(sum / num);
        };
    }

    constexpr auto sum()
    {
        return [](auto e) {
            using T = std::remove_const_t<std::remove_reference_t<decltype(*e.enumerator())>>;
            T sum{ 0 };
            for (auto item : e)
            {
                sum += item;
            }
            return sum;
        };
    }

    template <typename E2>
    constexpr auto equals(E2&& e2)
    {
        return [&](auto e) {
            auto eter1{ e.enumerator() };
            auto eter2{ get_enumerator(std::forward<E2>(e2)) };
            for (; eter1 && eter2; ++eter1, ++eter2)
            {
                if (*eter1 != *eter2)
                    return false;
            }
            return !eter1 && !eter2;
        };
    }

    namespace impl
    {
        template <typename T>
        class reverse_enumerator
        {
        private:
            std::vector<T> m_vec;
            typename std::vector<T>::reverse_iterator m_begin, m_end;

        public:
            // Prevent multi-reverse in copying.
            constexpr reverse_enumerator(const reverse_enumerator& eter) : m_vec(eter.m_vec), m_begin(m_vec.rbegin()), m_end(m_vec.rend()) {}

            template <typename Eter>
            constexpr reverse_enumerator(Eter&& eter)
            {
                for (; eter; ++eter)
                {
                    m_vec.emplace_back(*eter);
                }
                m_begin = m_vec.rbegin();
                m_end = m_vec.rend();
            }

            constexpr operator bool() const { return m_begin != m_end; }
            constexpr reverse_enumerator& operator++()
            {
                ++m_begin;
                return *this;
            }
            constexpr T operator*() { return *m_begin; }
        };
    } // namespace impl

    constexpr auto reverse()
    {
        return [](auto e) {
            using T = std::remove_const_t<std::remove_reference_t<decltype(*e.enumerator())>>;
            return enumerable(impl::reverse_enumerator<T>(e.enumerator()));
        };
    }

    namespace impl
    {
        constexpr auto always_identity()
        {
            return [](auto i) { return i; };
        }
    } // namespace impl

    constexpr auto ascending()
    {
        return [](auto l, auto r) { return l < r; };
    }

    constexpr auto descending()
    {
        return [](auto l, auto r) { return l > r; };
    }

    template <typename Selector = decltype(impl::always_identity()), typename Comparer = decltype(ascending())>
    constexpr auto sort(Selector&& selector = impl::always_identity(), Comparer&& comparer = ascending())
    {
        return [&](auto e) {
            using T = std::remove_const_t<std::remove_reference_t<decltype(*e.enumerator())>>;
            std::vector<T> result;
            for (auto item : e)
            {
                result.emplace_back(item);
            }
            std::sort(result.begin(), result.end(), [&](T& t1, T& t2) { return comparer(selector(t1), selector(t2)); });
            return get_enumerable(std::move(result));
        };
    }
} // namespace linq

#endif // !LINQ_AGGREGATE_HPP
