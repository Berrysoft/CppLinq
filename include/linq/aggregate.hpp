/**CppLinq aggregate.hpp
 * 
 * MIT License
 * 
 * Copyright (c) 2019 Berrysoft
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */
#ifndef LINQ_AGGREGATE_HPP
#define LINQ_AGGREGATE_HPP

#include <algorithm>
#include <linq/core.hpp>
#include <set>
#include <vector>

namespace linq
{
    struct always_true
    {
        template <typename T>
        constexpr bool operator()(T&&)
        {
            return true;
        }
    };

    // The count of the enumerable.
    // Returns the total count with the default parameter.
    template <typename Pred = always_true>
    constexpr auto count(Pred&& pred = {})
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

    // Determines whether all elements satisfy a condition.
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

    // Determines if any elements satisfy a condition.
    // empty() <=> !any()
    // contains(x) <=> any([](auto a) { return a == x; })
    template <typename Pred = always_true>
    constexpr auto any(Pred&& pred = {})
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

    // Applies an accumulator function over an enumerable.
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

    // Calculates the average value of the elements.
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

    // Calculates the sum of the elements.
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

    // reverse enumerator
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

    template <typename T>
    constexpr auto reverse()
    {
        return [](auto e) {
            return enumerable(impl::reverse_enumerator<T>(e.enumerator()));
        };
    }

    struct identity
    {
        template <typename T>
        constexpr T&& operator()(T&& t)
        {
            return std::forward<T>(t);
        }
    };

    // Sorts the enumerable by the specified selector and comparer.
    template <typename T, typename Selector = identity, typename Comparer = std::less<T>>
    constexpr auto sort(Selector&& selector = {}, Comparer&& comparer = {})
    {
        return [&](auto e) {
            std::vector<T> result;
            for (auto item : e)
            {
                result.emplace_back(item);
            }
            std::sort(result.begin(), result.end(), [&](T& t1, T& t2) { return comparer(selector(t1), selector(t2)); });
            return get_enumerable(std::move(result));
        };
    }

    // Gets the limit value of an enumerable.
    // min <=> limit(less<T>{})
    // max <=> limit(greater<T>{})
    template <typename T, typename Comparer>
    constexpr auto limit(Comparer&& comparer)
    {
        return [&](auto e) {
            auto eter{ e.enumerator() };
            if (!eter)
                return T{};
            T result = *eter;
            for (++eter; eter; ++eter)
            {
                if (!comparer(result, *eter))
                    result = *eter;
            }
            return result;
        };
    }

    constexpr auto get_at(std::size_t index)
    {
        return [=](auto e) {
            auto eter{ e.enumerator() };
            for (std::size_t i{ 0 }; eter && i < index; ++eter, ++i)
                ;
            return *eter;
        };
    }

    namespace impl
    {
        template <typename T, typename Eter>
        class distinct_enumerator
        {
        private:
            std::set<T> m_set;
            Eter m_eter;

            constexpr void move_next()
            {
                for (; m_eter; ++m_eter)
                {
                    if (m_set.emplace(*m_eter).second)
                        break;
                }
            }

        public:
            constexpr distinct_enumerator(Eter&& eter) : m_eter(std::forward<Eter>(eter))
            {
                move_next();
            }

            constexpr operator bool() const { return m_eter; }
            constexpr distinct_enumerator& operator++()
            {
                move_next();
                return *this;
            }
            constexpr decltype(auto) operator*() { return *m_eter; }
        };
    } // namespace impl

    template <typename T>
    constexpr auto distinct()
    {
        return [](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::distinct_enumerator<T, Eter>(e.enumerator()));
        };
    }

    namespace impl
    {
        template <typename T, typename Eter1, typename Eter2>
        class union_set_enumerator
        {
        private:
            std::set<T> m_set;
            Eter1 m_eter1;
            Eter2 m_eter2;

            template <typename Eter>
            constexpr bool move_next(Eter& eter)
            {
                for (; eter; ++eter)
                {
                    if (m_set.emplace(*eter).second)
                        return true;
                }
                return false;
            }

            constexpr void move_next()
            {
                if (!m_eter1 || (m_eter1 && !move_next(m_eter1)))
                    move_next(m_eter2);
            }

        public:
            constexpr union_set_enumerator(Eter1&& eter1, Eter2&& eter2) : m_eter1(std::forward<Eter1>(eter1)), m_eter2(std::forward<Eter2>(eter2))
            {
                move_next();
            }

            constexpr operator bool() const { return m_eter1 || m_eter2; }
            constexpr union_set_enumerator& operator++()
            {
                move_next();
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_eter1 ? *m_eter1 : *m_eter2; }
        };
    } // namespace impl

    template <typename T, typename E2>
    constexpr auto union_set(E2&& e2)
    {
        return [&](auto e) {
            using Eter1 = decltype(e.enumerator());
            using Eter2 = decltype(get_enumerator(std::forward<E2>(e2)));
            return enumerable(impl::union_set_enumerator<T, Eter1, Eter2>(e.enumerator(), get_enumerator(std::forward<E2>(e2))));
        };
    }

    namespace impl
    {
        template <typename T, typename Eter1>
        class intersect_enumerator
        {
        private:
            std::set<T> m_set;
            Eter1 m_eter1;

            constexpr void move_next()
            {
                for (; m_eter1; ++m_eter1)
                {
                    if (m_set.erase(*m_eter1))
                        break;
                }
            }

        public:
            template <typename Eter2>
            constexpr intersect_enumerator(Eter1&& eter1, Eter2&& eter2) : m_eter1(std::forward<Eter1>(eter1))
            {
                for (; eter2; ++eter2)
                {
                    m_set.emplace(*eter2);
                }
                move_next();
            }

            constexpr operator bool() const { return m_eter1; }
            constexpr intersect_enumerator& operator++()
            {
                move_next();
                return *this;
            }
            constexpr decltype(auto) operator*() { return *m_eter1; }
        };
    } // namespace impl

    template <typename T, typename E2>
    constexpr auto intersect(E2&& e2)
    {
        return [&](auto e) {
            using Eter1 = decltype(e.enumerator());
            return enumerable(impl::intersect_enumerator<T, Eter1>(e.enumerator(), get_enumerator(std::forward<E2>(e2))));
        };
    }

    namespace impl
    {
        template <typename T, typename Eter1>
        class except_enumerator
        {
        private:
            std::set<T> m_set;
            Eter1 m_eter1;

            constexpr void move_next()
            {
                for (; m_eter1; ++m_eter1)
                {
                    if (m_set.emplace(*m_eter1).second)
                        break;
                }
            }

        public:
            template <typename Eter2>
            constexpr except_enumerator(Eter1&& eter1, Eter2&& eter2) : m_eter1(std::forward<Eter1>(eter1))
            {
                for (; eter2; ++eter2)
                {
                    m_set.emplace(*eter2);
                }
                move_next();
            }

            constexpr operator bool() const { return m_eter1; }
            constexpr except_enumerator& operator++()
            {
                move_next();
                return *this;
            }
            constexpr decltype(auto) operator*() { return *m_eter1; }
        };
    } // namespace impl

    template <typename T, typename E2>
    constexpr auto except(E2&& e2)
    {
        return [&](auto e) {
            using Eter1 = decltype(e.enumerator());
            return enumerable(impl::except_enumerator<T, Eter1>(e.enumerator(), get_enumerator(std::forward<E2>(e2))));
        };
    }
} // namespace linq

#endif // !LINQ_AGGREGATE_HPP
