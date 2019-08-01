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
#include <deque>
#include <linq/core.hpp>
#include <map>
#include <set>
#include <string_view>

namespace linq
{
    // Applies an action to each element and return the enumerable.
    template <typename Action>
    constexpr auto for_each(Action&& action)
    {
        return [&](auto e) {
            auto eter{ e.enumerator() };
            for (; eter; ++eter)
                action(*eter);
            return e;
        };
    }

    // Applies an action to each element and return the enumerable.
    template <typename Action>
    constexpr auto for_each_index(Action&& action)
    {
        return [&](auto e) {
            auto eter{ e.enumerator() };
            for (std::size_t index{ 0 }; eter; ++eter, ++index)
                action(*eter, index);
            return e;
        };
    }

    namespace impl
    {
        template <typename Eter, typename Action>
        class peek_enumerator
        {
        private:
            Eter m_eter;
            Action m_action;

        public:
            constexpr peek_enumerator(Eter&& eter, Action&& action) : m_eter(std::forward<Eter>(eter)), m_action(action) {}

            constexpr operator bool() const { return m_eter; }
            constexpr peek_enumerator& operator++()
            {
                ++m_eter;
                return *this;
            }
            constexpr decltype(auto) operator*()
            {
                auto current{ *m_eter };
                m_action(current);
                return current;
            }
        };
    } // namespace impl

    // Applies an action to each element while enumerating.
    template <typename Action>
    constexpr auto peek(Action&& action)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::peek_enumerator<Eter, Action>(e.enumerator(), std::forward<Action>(action)));
        };
    }

    namespace impl
    {
        template <typename Eter, typename Action>
        class peek_index_enumerator
        {
        private:
            Eter m_eter;
            Action m_action;
            std::size_t m_index;

        public:
            constexpr peek_index_enumerator(Eter&& eter, Action&& action) : m_eter(std::forward<Eter>(eter)), m_action(action), m_index(0) {}

            constexpr operator bool() const { return m_eter; }
            constexpr peek_index_enumerator& operator++()
            {
                ++m_eter;
                ++m_index;
                return *this;
            }
            constexpr decltype(auto) operator*()
            {
                auto current{ *m_eter };
                m_action(current, m_index);
                return current;
            }
        };
    } // namespace impl

    template <typename Action>
    constexpr auto peek_index(Action&& action)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::peek_index_enumerator<Eter, Action>(e.enumerator(), std::forward<Action>(action)));
        };
    }

    // Always returns true.
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

    constexpr auto empty()
    {
        return [&](auto e) { !any()(e); };
    }

    template <typename T, typename Comparer = std::equal_to<void>>
    constexpr auto contains(T&& value, Comparer&& comparer = {})
    {
        return any([&](auto&& a) { return std::forward<Comparer>(comparer)(a, value); });
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

    // Returns the first element that satisfies a condition or a default value if no such element is found.
    template <typename Pred = always_true, typename T>
    constexpr auto front(Pred&& pred = {}, T&& def = {})
    {
        return [&](auto e) {
            auto eter{ e.enumerator() };
            for (; eter; ++eter)
            {
                if (pred(*eter))
                    return *eter;
            }
            return std::forward<T>(def);
        };
    }

    // Returns the first element that satisfies a specified condition.
    template <typename Pred = always_true>
    constexpr auto front(Pred&& pred = {})
    {
        return [&](auto e) {
            using T = remove_cref<decltype(*e.enumerator())>;
            return front<Pred, T>(std::forward<Pred>(pred))(e);
        };
    }

    // Returns the last element that satisfies a condition or a default value if no such element is found.
    template <typename Pred = always_true, typename T>
    constexpr auto back(Pred&& pred = {}, T&& def = {})
    {
        return [&](auto e) {
            auto eter{ e.enumerator() };
            T result{ std::forward<T>(def) };
            for (; eter; ++eter)
            {
                if (pred(*eter))
                    result = *eter;
            }
            return result;
        };
    }

    // Returns the last element that satisfies a specified condition.
    template <typename Pred = always_true>
    constexpr auto back(Pred&& pred = {})
    {
        return [&](auto e) {
            using T = remove_cref<decltype(*e.enumerator())>;
            return back<Pred, T>(std::forward<Pred>(pred))(e);
        };
    }

    struct more_than_one : std::logic_error
    {
        more_than_one() : logic_error("More than one element satisfies the condition.") {}
    };

    // Returns the only element that satisfies a specified condition or a default value if no such element exists;
    // this method throws an exception if more than one element satisfies the condition.
    template <typename Pred = always_true, typename T>
    constexpr auto single(Pred&& pred = {}, T&& def = {})
    {
        return [&](auto e) {
            auto eter{ e.enumerator() };
            T result{ std::forward<T>(def) };
            std::size_t num{ 0 };
            for (; eter; ++eter)
            {
                if (pred(*eter))
                {
                    result = *eter;
                    ++num;
                }
            }
            switch (num)
            {
            case 0:
            case 1:
                return result;
            default:
                throw more_than_one{};
            }
        };
    }

    // Returns the only element that satisfies a specified condition or a default value if no such element exists;
    // this method throws an exception if more than one element satisfies the condition.
    template <typename Pred = always_true>
    constexpr auto single(Pred&& pred = {})
    {
        return [&](auto e) {
            using T = remove_cref<decltype(*e.enumerator())>;
            return single<Pred, T>(std::forward<Pred>(pred))(e);
        };
    }

    namespace impl
    {
        template <typename Eter, typename T>
        class default_if_empty_enumerator
        {
        private:
            Eter m_eter;
            std::optional<T> m_def;

        public:
            constexpr default_if_empty_enumerator(Eter&& eter, T&& t) : m_eter(std::forward<Eter>(eter))
            {
                if (!m_eter)
                    m_def.emplace(std::forward<T>(t));
            }

            constexpr operator bool() const { return m_eter || m_def; }
            constexpr default_if_empty_enumerator& operator++()
            {
                if (!m_eter)
                    m_def = std::nullopt;
                else
                    ++m_eter;
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_eter ? *m_eter : *m_def; }
        };
    } // namespace impl

    // Returns the elements of the specified enumerable or the element type's default value in a singleton collection if the enumerable is empty.
    constexpr auto default_if_empty()
    {
        return [](auto e) {
            using Eter = decltype(e.enumerator());
            using T = remove_cref<decltype(*e.enumerator())>;
            return enumerable(impl::default_if_empty_enumerator<Eter, T>(e.enumerator(), T{}));
        };
    }

    // Returns the elements of the specified enumerable or the specified value in a singleton collection if the enumerable is empty.
    template <typename T>
    constexpr auto default_if_empty(T&& def)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::default_if_empty_enumerator<Eter, T>(e.enumerator(), std::forward<T>(def)));
        };
    }

    // Calculates the average value of the elements.
    constexpr auto average()
    {
        return [](auto e) {
            using T = remove_cref<decltype(*e.enumerator())>;
            std::size_t num{ 0 };
            return (T)(aggregate<T>({}, [&num](auto& a, auto& b) { ++num; return a + b; })(e) / num);
        };
    }

    // Calculates the sum of the elements.
    constexpr auto sum()
    {
        return [](auto e) {
            using T = remove_cref<decltype(*e.enumerator())>;
            return aggregate<T>({}, [](auto& a, auto& b) { return a + b; })(e);
        };
    }

    namespace impl
    {
        template <typename T>
        class reverse_enumerator
        {
        private:
            std::deque<T> m_vec;
            typename std::deque<T>::iterator m_begin, m_end;

        public:
            // Prevent multi-reverse in copying.
            constexpr reverse_enumerator( reverse_enumerator&& eter) {
              m_vec.swap(eter.m_vec); 
              m_begin=(m_vec.begin());
              m_end=(m_vec.end());
            }
            constexpr reverse_enumerator(const reverse_enumerator& eter) :
              m_vec(eter.m_vec), 
              m_begin(m_vec.begin()),
              m_end(m_vec.end()) {}
            constexpr reverse_enumerator& operator=(const reverse_enumerator& eter)
            {
                m_vec = eter.m_vec;
                m_begin = m_vec.m_begin;
                m_end = m_vec.m_end;
                return *this;
            }

            template <typename Eter>
            constexpr reverse_enumerator(Eter&& eter)
            {
                //std::cout << "RE Ctor "<< std::endl;
                for (; eter; ++eter)
                {
                    m_vec.emplace_front(*eter);
                }
                m_begin = m_vec.begin();
                m_end = m_vec.end();
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

    // Inverts the order of the elements.
    constexpr auto reverse()
    {
        return [](auto e) {
            using T = remove_cref<decltype(*e.enumerator())>;
            return enumerable(impl::reverse_enumerator<T>(e.enumerator()));
        };
    }

    // Always returns the param itself.
    struct identity
    {
        template <typename T>
        constexpr T&& operator()(T&& t)
        {
            return std::forward<T>(t);
        }
    };

    // Compare numeric elements.
    struct ascending
    {
        template <typename T1, typename T2>
        constexpr auto operator()(T1&& t1, T2&& t2) const
        {
            return (t1 > t2) - (t1 < t2);
        }
    };

    // Compare string elements.
    template <typename Char, typename Traits = std::char_traits<Char>>
    struct string_ascending
    {
        using string_view_type = std::basic_string_view<Char, Traits>;

        template <typename T1, typename T2>
        constexpr auto operator()(T1&& t1, T2&& t2) const
        {
            string_view_type s1{ std::forward<T1>(t1) };
            string_view_type s2{ std::forward<T2>(t2) };
            return s1.compare(s2);
        }
    };

    // Compare numeric elements.
    struct descending
    {
        template <typename T1, typename T2>
        constexpr auto operator()(T1&& t1, T2&& t2) const
        {
            return (t1 < t2) - (t1 > t2);
        }
    };

    // Compare string elements.
    template <typename Char, typename Traits = std::char_traits<Char>>
    struct string_descending
    {
        using string_view_type = std::basic_string_view<Char, Traits>;

        template <typename T1, typename T2>
        constexpr auto operator()(T1&& t1, T2&& t2) const
        {
            string_view_type s1{ std::forward<T1>(t1) };
            string_view_type s2{ std::forward<T2>(t2) };
            return s2.compare(s1);
        }
    };

    // Make a comparer with a selector and a ascending/descending comparer.
    template <typename Selector = identity, typename Comparer = ascending>
    constexpr auto make_comparer(Selector&& selector = {}, Comparer&& comparer = {})
    {
        return [&](auto&& t1, auto&& t2) { return comparer(selector(t1), selector(t2)); };
    }

    namespace impl
    {
        template <typename C1, typename... Comparer>
        constexpr auto consume_comparer(C1&& c1, Comparer&&... comparer)
        {
            return [&](auto&& t1, auto&& t2) {
                auto t{ std::forward<C1>(c1)(t1, t2) };
                if constexpr (sizeof...(Comparer))
                {
                    if (t == 0)
                        return consume_comparer<Comparer...>(std::forward<Comparer>(comparer)...)(t1, t2);
                }
                return t < 0;
            };
        }
    } // namespace impl

    // Makes a sorter for algorithms.
    template <typename... Comparer>
    constexpr auto make_sorter(Comparer&&... comparer)
    {
        if constexpr (sizeof...(Comparer) == 0)
        {
            return impl::consume_comparer(ascending{});
        }
        else
        {
            return impl::consume_comparer<Comparer...>(std::forward<Comparer>(comparer)...);
        }
    }

    // Sorts the enumerable by the specified comparer.
    template <typename... Comparer>
    constexpr auto sort(Comparer&&... comparer)
    {
        return [&](auto e) {
            using T = remove_cref<decltype(*e.enumerator())>;
            std::deque<T> result(e.begin(), e.end());
            std::sort(result.begin(), result.end(), make_sorter<Comparer...>(std::forward<Comparer>(comparer)...));
            return get_enumerable(std::move(result));
        };
    }

    // Gets the limit value of an enumerable.
    template <typename Comparer>
    constexpr auto limit(Comparer&& comparer)
    {
        return [&](auto e) {
            using T = remove_cref<decltype(*e.enumerator())>;
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

    template <typename Comparer, typename T>
    constexpr auto limit(Comparer&& comparer, T&& def)
    {
        return [&](auto e) {
            auto eter{ e.enumerator() };
            T result{ std::forward<T>(def) };
            for (; eter; ++eter)
            {
                if (!comparer(result, *eter))
                    result = *eter;
            }
            return result;
        };
    }

    constexpr auto(min)()
    {
        return limit<std::less<void>>({});
    }

    template <typename T>
    constexpr auto(min)(T&& def)
    {
        return limit<std::less<void>, T>({}, std::forward<T>(def));
    }

    constexpr auto(max)()
    {
        return limit<std::greater<void>>({});
    }

    template <typename T>
    constexpr auto(max)(T&& def)
    {
        return limit<std::greater<void>, T>({}, std::forward<T>(def));
    }

    // Returns the element at a specified index.
    template <typename T>
    constexpr auto get_at(std::size_t index, T&& def)
    {
        return [index, &def](auto e) {
            auto eter{ e.enumerator() };
            for (std::size_t i{ 0 }; eter && i < index; ++eter, ++i)
                ;
            if (!eter)
                return std::forward<T>(def);
            else
                return *eter;
        };
    }

    // Returns the element at a specified index.
    constexpr auto get_at(std::size_t index)
    {
        return [=](auto e) {
            using T = remove_cref<decltype(*e.enumerator())>;
            return get_at<T>(index, {})(e);
        };
    }

    // Returns the index of the first element which satifies the predicate.
    template <typename Pred>
    constexpr auto index_of(Pred&& pred)
    {
        return [&](auto e) {
            auto eter{ e.enumerator() };
            std::size_t index{ 0 };
            for (; eter && !pred(*eter); ++eter, ++index)
                ;
            if (!eter)
                return static_cast<std::size_t>(-1);
            else
                return index;
        };
    }

    namespace impl
    {
        template <typename Eter, typename T, typename Comparer>
        class distinct_enumerator
        {
        private:
            std::set<T, Comparer> m_set;
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

    // Returns distinct elements.
    template <typename Comparer = std::less<void>>
    constexpr auto distinct()
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            using T = remove_cref<decltype(*e.enumerator())>;
            return enumerable(impl::distinct_enumerator<Eter, T, Comparer>(e.enumerator()));
        };
    }

    namespace impl
    {
        template <typename Eter1, typename Eter2, typename T, typename Comparer>
        class union_set_enumerator
        {
        private:
            std::set<T, Comparer> m_set;
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

    // Produces the set union of two enumerable.
    template <typename Comparer = std::less<void>, typename E2>
    constexpr auto union_set(E2&& e2)
    {
        return [&](auto e) {
            using Eter1 = decltype(e.enumerator());
            using Eter2 = decltype(get_enumerator(std::forward<E2>(e2)));
            using T = remove_cref<decltype(*e.enumerator())>;
            return enumerable(impl::union_set_enumerator<Eter1, Eter2, T, Comparer>(e.enumerator(), get_enumerator(std::forward<E2>(e2))));
        };
    }

    namespace impl
    {
        template <typename Eter1, typename T, typename Comparer>
        class intersect_enumerator
        {
        private:
            std::set<T, Comparer> m_set;
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

    // Produces the set intersection of two enumerable.
    template <typename Comparer = std::less<void>, typename E2>
    constexpr auto intersect(E2&& e2)
    {
        return [&](auto e) {
            using Eter1 = decltype(e.enumerator());
            using T = remove_cref<decltype(*e.enumerator())>;
            return enumerable(impl::intersect_enumerator<Eter1, T, Comparer>(e.enumerator(), get_enumerator(std::forward<E2>(e2))));
        };
    }

    namespace impl
    {
        template <typename Eter1, typename T, typename Comparer>
        class except_enumerator
        {
        private:
            std::set<T, Comparer> m_set;
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

    // Produces the set difference of two enumerable.
    template <typename Comparer = std::less<void>, typename E2>
    constexpr auto except(E2&& e2)
    {
        return [&](auto e) {
            using Eter1 = decltype(e.enumerator());
            using T = remove_cref<decltype(*e.enumerator())>;
            return enumerable(impl::except_enumerator<Eter1, T, Comparer>(e.enumerator(), get_enumerator(std::forward<E2>(e2))));
        };
    }

    namespace impl
    {
        template <typename TKey, typename TElement, typename ResultSelector, typename Comparer>
        class group_enumerator
        {
        private:
            std::map<TKey, std::deque<TElement>, Comparer> m_lookup;
            typename std::map<TKey, std::deque<TElement>, Comparer>::iterator m_begin, m_end;
            ResultSelector m_rstsel;

        public:
            constexpr group_enumerator(const group_enumerator& eter) : m_lookup(eter.m_lookup), m_begin(m_lookup.begin()), m_end(m_lookup.end()), m_rstsel(eter.m_rstsel) {}
            constexpr group_enumerator& operator=(const group_enumerator& eter)
            {
                m_lookup = eter.m_lookup;
                m_begin = m_lookup.begin();
                m_end = m_lookup.end();
                m_rstsel = eter.m_rstsel;
                return *this;
            }

            template <typename Eter, typename KeySelector, typename ElementSelector>
            constexpr group_enumerator(Eter&& eter, KeySelector&& keysel, ElementSelector&& elesel, ResultSelector&& rstsel) : m_rstsel(rstsel)
            {
                for (; eter; ++eter)
                {
                    m_lookup[keysel(*eter)].emplace_back(elesel(*eter));
                }
                m_begin = m_lookup.begin();
                m_end = m_lookup.end();
            }

            constexpr operator bool() const { return m_begin != m_end; }
            constexpr group_enumerator& operator++()
            {
                ++m_begin;
                return *this;
            }
            constexpr decltype(auto) operator*()
            {
                auto&& f{ m_begin->first };
                return m_rstsel(f, get_enumerable(m_lookup[f]));
            }
        };
    } // namespace impl

    // Groups the elements according to a specified key selector function and creates a result value from each group and its key.
    // Key values are compared by using a specified comparer, and the elements of each group are projected by using a specified function.
    template <typename Comparer = std::less<void>, typename KeySelector, typename ElementSelector, typename ResultSelector>
    constexpr auto group(KeySelector&& keysel, ElementSelector&& elesel, ResultSelector&& rstsel)
    {
        return [&](auto e) {
            using TKey = remove_cref<decltype(keysel(*e.enumerator()))>;
            using TElement = remove_cref<decltype(elesel(*e.enumerator()))>;
            return enumerable(impl::group_enumerator<TKey, TElement, ResultSelector, Comparer>(e.enumerator(), std::forward<KeySelector>(keysel), std::forward<ElementSelector>(elesel), std::forward<ResultSelector>(rstsel)));
        };
    }

    namespace impl
    {
        template <typename Eter, typename TKey, typename TElement, typename KeySelector, typename ResultSelector, typename Comparer>
        class group_join_enumerator
        {
        private:
            Eter m_eter;
            std::map<TKey, std::deque<TElement>, Comparer> m_lookup;
            KeySelector m_keysel;
            ResultSelector m_rstsel;

        public:
            template <typename Eter2, typename KeySelector2, typename ElementSelector2>
            constexpr group_join_enumerator(Eter&& eter, Eter2&& eter2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
                : m_eter(std::forward<Eter>(eter)), m_keysel(keysel), m_rstsel(rstsel)
            {
                for (; eter2; ++eter2)
                {
                    m_lookup[keysel2(*eter2)].emplace_back(elesel2(*eter2));
                }
            }

            constexpr operator bool() const { return m_eter; }
            constexpr group_join_enumerator& operator++()
            {
                ++m_eter;
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_rstsel(*m_eter, get_enumerable(m_lookup[m_keysel(*m_eter)])); }
        };
    } // namespace impl

    // Correlates the elements of two enumerable based on key comparer and groups the results.
    template <typename Comparer = std::less<void>, typename E2, typename KeySelector, typename KeySelector2, typename ElementSelector2, typename ResultSelector>
    constexpr auto group_join(E2&& e2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            using TKey = remove_cref<decltype(keysel2(*get_enumerator(e2)))>;
            using TElement = remove_cref<decltype(elesel2(*get_enumerator(e2)))>;
            return enumerable(impl::group_join_enumerator<Eter, TKey, TElement, KeySelector, ResultSelector, Comparer>(e.enumerator(), get_enumerator(std::forward<E2>(e2)), std::forward<KeySelector>(keysel), std::forward<KeySelector2>(keysel2), std::forward<ElementSelector2>(elesel2), std::forward<ResultSelector>(rstsel)));
        };
    }

    namespace impl
    {
        template <typename Eter, typename TKey, typename TElement, typename KeySelector, typename ResultSelector, typename Comparer>
        class join_enumerator
        {
        private:
            Eter m_eter;
            std::map<TKey, std::deque<TElement>, Comparer> m_lookup;
            typename std::deque<TElement>::iterator m_begin, m_end;
            KeySelector m_keysel;
            ResultSelector m_rstsel;

            constexpr void move_next()
            {
                if (m_eter)
                {
                    decltype(auto) deq = m_lookup[m_keysel(*m_eter)];
                    m_begin = deq.begin();
                    m_end = deq.end();
                }
            }

        public:
            template <typename Eter2, typename KeySelector2, typename ElementSelector2>
            constexpr join_enumerator(Eter&& eter, Eter2&& eter2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
                : m_eter(std::forward<Eter>(eter)), m_keysel(keysel), m_rstsel(rstsel)
            {
                for (; eter2; ++eter2)
                {
                    m_lookup[keysel2(*eter2)].emplace_back(elesel2(*eter2));
                }
                move_next();
            }

            constexpr operator bool() const { return m_eter || m_begin != m_end; }
            constexpr join_enumerator& operator++()
            {
                ++m_begin;
                if (m_begin == m_end)
                {
                    ++m_eter;
                    move_next();
                }
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_rstsel(*m_eter, *m_begin); }
        };
    } // namespace impl

    // Correlates the elements of two enumerable based on matching keys.
    template <typename Comparer = std::less<void>, typename E2, typename KeySelector, typename KeySelector2, typename ElementSelector2, typename ResultSelector>
    constexpr auto join(E2&& e2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            using TKey = remove_cref<decltype(keysel2(*get_enumerator(e2)))>;
            using TElement = remove_cref<decltype(elesel2(*get_enumerator(e2)))>;
            return enumerable(impl::join_enumerator<Eter, TKey, TElement, KeySelector, ResultSelector, Comparer>(e.enumerator(), get_enumerator(std::forward<E2>(e2)), std::forward<KeySelector>(keysel), std::forward<KeySelector2>(keysel2), std::forward<ElementSelector2>(elesel2), std::forward<ResultSelector>(rstsel)));
        };
    }
} // namespace linq

#endif // !LINQ_AGGREGATE_HPP
