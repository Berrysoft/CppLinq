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

    // Determines whether the two enumerable are equal.
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
    template <typename T, typename Selector = identity, typename Comparer = less<T>>
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
} // namespace linq

#endif // !LINQ_AGGREGATE_HPP
