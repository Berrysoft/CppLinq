/**CppLinq aggregate.hpp
 * 
 * MIT License
 * 
 * Copyright (c) 2019-2020 Berrysoft
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
#include <map>
#include <set>
#include <string_view>
#include <vector>

namespace linq
{
    // Applies an action to each element and return the enumerable.
    template <typename Action>
    constexpr auto for_each(Action&& action)
    {
        return [&](auto&& container) {
            for (auto& item : container)
                action(item);
            return container;
        };
    }

    // Applies an action to each element and return the enumerable.
    template <typename Action>
    constexpr auto for_each_index(Action&& action)
    {
        return [&](auto&& container) {
            std::size_t i = 0;
            for (auto& item : container)
            {
                action(item, i);
                i++;
            }
            return container;
        };
    }

    namespace impl
    {
        template <typename It, typename Action>
        class peek_iterator_impl
        {
        private:
            It m_begin, m_end;
            std::decay_t<Action> m_action;

            using value_type = typename std::iterator_traits<It>::value_type;
            std::optional<value_type> m_result;

            void set_result()
            {
                if (m_begin != m_end)
                    m_result = *m_begin;
            }

        public:
            using traits_type = std::iterator_traits<It>;

            peek_iterator_impl(It begin, It end, Action&& action) : m_begin(begin), m_end(end), m_action(std::forward<Action>(action)) { set_result(); }

            typename traits_type::reference value()
            {
                m_action(*m_result);
                return *m_result;
            }

            void move_next()
            {
                ++m_begin;
                set_result();
            }

            bool is_valid() const { return m_begin != m_end; }
        };

        template <typename It, typename Action>
        using peek_iterator = iterator_base<peek_iterator_impl<It, Action>>;
    } // namespace impl

    // Applies an action to each element while enumerating.
    template <typename Action>
    constexpr auto peek(Action&& action)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::peek_iterator<It, Action>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<Action>(action) } };
        };
    }

    namespace impl
    {
        template <typename It, typename Action>
        class peek_index_iterator_impl
        {
        private:
            It m_begin, m_end;
            std::decay_t<Action> m_action;
            std::size_t m_index{ 0 };

            using value_type = typename std::iterator_traits<It>::value_type;
            std::optional<value_type> m_result;

            void set_result()
            {
                if (m_begin != m_end)
                    m_result = *m_begin;
            }

        public:
            using traits_type = std::iterator_traits<It>;

            peek_index_iterator_impl(It begin, It end, Action&& action) : m_begin(begin), m_end(end), m_action(std::forward<Action>(action)) { set_result(); }

            typename traits_type::reference value()
            {
                m_action(*m_result, m_index);
                return *m_result;
            }

            void move_next()
            {
                ++m_begin;
                ++m_index;
                set_result();
            }

            bool is_valid() const { return m_begin != m_end; }
        };

        template <typename It, typename Action>
        using peek_index_iterator = iterator_base<peek_index_iterator_impl<It, Action>>;
    } // namespace impl

    template <typename Action>
    constexpr auto peek_index(Action&& action)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::peek_index_iterator<It, Action>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<Action>(action) } };
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
        return [&](auto&& container) {
            std::size_t result{ 0 };
            for (auto& item : container)
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
        return [&](auto&& container) {
            for (auto& item : container)
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
        return [&](auto&& container) {
            for (auto& item : container)
            {
                if (pred(item))
                    return true;
            }
            return false;
        };
    }

    constexpr auto empty()
    {
        return [](auto&& container) {
            auto begin = std::begin(container);
            auto end = std::end(container);
            return begin == end;
        };
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
        return [&](auto&& container) {
            T result{ seed };
            for (auto& item : container)
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
        return [&](auto&& container) {
            for (auto& item : container)
            {
                if (pred(item)) return item;
            }
            return std::forward<T>(def);
        };
    }

    // Returns the first element that satisfies a specified condition.
    template <typename Pred = always_true>
    constexpr auto front(Pred&& pred = {})
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            return front<Pred, T>(std::forward<Pred>(pred))(container);
        };
    }

    // Returns the last element that satisfies a condition or a default value if no such element is found.
    template <typename Pred = always_true, typename T>
    constexpr auto back(Pred&& pred = {}, T&& def = {})
    {
        return [&](auto&& container) {
            T result{ std::forward<T>(def) };
            for (auto& item : container)
            {
                if (pred(item)) result = item;
            }
            return result;
        };
    }

    // Returns the last element that satisfies a specified condition.
    template <typename Pred = always_true>
    constexpr auto back(Pred&& pred = {})
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            return back<Pred, T>(std::forward<Pred>(pred))(container);
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
        return [&](auto&& container) {
            T result{ std::forward<T>(def) };
            std::size_t num{ 0 };
            for (auto& item : container)
            {
                if (pred(item))
                {
                    result = item;
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
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            return single<Pred, T>(std::forward<Pred>(pred))(container);
        };
    }

    namespace impl
    {
        template <typename It, typename T>
        class default_if_empty_iterator_impl
        {
        private:
            It m_begin, m_end;
            std::optional<T> m_def;

        public:
            using traits_type = iterator_impl_traits<std::common_type_t<T, typename std::iterator_traits<It>::value_type>>;

            default_if_empty_iterator_impl(It begin, It end, T&& t) : m_begin(begin), m_end(end)
            {
                if (m_begin == m_end)
                    m_def.emplace(std::forward<T>(t));
            }

            typename traits_type::reference value() const { return m_begin != m_end ? *m_begin : *m_def; }

            void move_next()
            {
                if (m_def)
                    m_def = std::nullopt;
                else
                    ++m_begin;
            }

            bool is_valid() const { return m_begin != m_end || m_def; }
        };

        template <typename It, typename T>
        using default_if_empty_iterator = iterator_base<default_if_empty_iterator_impl<It, T>>;
    } // namespace impl

    // Returns the elements of the specified enumerable or the element type's default value in a singleton collection if the enumerable is empty.
    constexpr auto default_if_empty()
    {
        return [](auto&& container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            return impl::iterable{ impl::default_if_empty_iterator<It, T>{ impl::iterator_ctor, std::begin(container), std::end(container), T{} } };
        };
    }

    // Returns the elements of the specified enumerable or the specified value in a singleton collection if the enumerable is empty.
    template <typename T>
    constexpr auto default_if_empty(T&& def)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::default_if_empty_iterator<It, T>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<T>(def) } };
        };
    }

    // Calculates the average value of the elements.
    constexpr auto average()
    {
        return [](auto&& container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            std::size_t num{ 0 };
            return aggregate<T>({}, [&num](auto& a, auto& b) { ++num; return a + b; })(container) / static_cast<T>(num);
        };
    }

    // Calculates the sum of the elements.
    constexpr auto sum()
    {
        return [](auto&& container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            return aggregate<T>({}, [](auto& a, auto& b) { return a + b; })(container);
        };
    }

    namespace impl
    {
        template <typename T>
        class reverse_iterator_impl
        {
        private:
            std::vector<T> m_vec;
            typename std::vector<T>::const_reverse_iterator m_begin, m_end;

        public:
            using traits_type = iterator_impl_traits<T>;

            template <typename It>
            reverse_iterator_impl(It begin, It end) : m_vec(begin, end)
            {
                m_begin = m_vec.crbegin();
                m_end = m_vec.crend();
            }

            typename traits_type::reference value() const { return *m_begin; }

            void move_next() { ++m_begin; }

            bool is_valid() const { return m_begin != m_end; }
        };

        template <typename T>
        using reverse_iterator = iterator_base<reverse_iterator_impl<T>>;
    } // namespace impl

    // Inverts the order of the elements.
    constexpr auto reverse()
    {
        return [](auto&& container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            return impl::iterable{ impl::reverse_iterator<T>{ impl::iterator_ctor, std::begin(container), std::end(container) } };
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
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            std::vector<T> result(std::begin(container), std::end(container));
            std::sort(result.begin(), result.end(), make_sorter<Comparer...>(std::forward<Comparer>(comparer)...));
            return result;
        };
    }

    // Gets the limit value of an enumerable.
    template <typename Comparer>
    constexpr auto limit(Comparer&& comparer)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            auto begin = std::begin(container);
            auto end = std::end(container);
            if (begin == end)
                return T{};
            T result = *begin;
            for (++begin; begin != end; ++begin)
            {
                if (!comparer(result, *begin))
                    result = *begin;
            }
            return result;
        };
    }

    template <typename Comparer, typename T>
    constexpr auto limit(Comparer&& comparer, T&& def)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            T result{ std::forward<T>(def) };
            for (auto& item : container)
            {
                if (!comparer(result, item))
                    result = item;
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
        return [index, &def](auto&& container) {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (std::size_t i{ 0 }; begin != end && i < index; ++begin, ++i)
                ;
            if (begin == end)
                return std::forward<T>(def);
            else
                return *begin;
        };
    }

    // Returns the element at a specified index.
    constexpr auto get_at(std::size_t index)
    {
        return [=](auto&& container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            return get_at<T>(index, {})(container);
        };
    }

    // Returns the index of the first element which satifies the predicate.
    template <typename Pred>
    constexpr auto index_of(Pred&& pred)
    {
        return [&](auto&& container) {
            std::size_t index{ 0 };
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && !pred(*begin); ++begin, ++index)
                ;
            if (begin == end)
                return static_cast<std::size_t>(-1);
            else
                return index;
        };
    }

    namespace impl
    {
        template <typename It, typename Comparer>
        class distinct_iterator_impl
        {
        private:
            using value_type = typename std::iterator_traits<It>::value_type;

            std::set<value_type, Comparer> m_set;
            It m_begin, m_end;

        public:
            using traits_type = std::iterator_traits<It>;

            distinct_iterator_impl(It begin, It end) : m_begin(begin), m_end(end) { move_next(); }

            typename traits_type::reference value() { return *m_begin; }

            void move_next()
            {
                for (; m_begin != m_end; ++m_begin)
                {
                    if (m_set.emplace(*m_begin).second)
                        break;
                }
            }

            bool is_valid() const { return m_begin != m_end; }
        };

        template <typename It, typename Comparer>
        using distinct_iterator = iterator_base<distinct_iterator_impl<It, Comparer>>;
    } // namespace impl

    // Returns distinct elements.
    template <typename Comparer = std::less<void>>
    constexpr auto distinct()
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::distinct_iterator<It, Comparer>{ impl::iterator_ctor, std::begin(container), std::end(container) } };
        };
    }

    //namespace impl
    //{
    //    template <typename Eter1, typename Eter2, typename T, typename Comparer>
    //    class union_set_enumerator
    //    {
    //    private:
    //        std::set<T, Comparer> m_set;
    //        Eter1 m_eter1;
    //        Eter2 m_eter2;

    //        template <typename Eter>
    //        constexpr bool move_next(Eter& eter)
    //        {
    //            for (; eter; ++eter)
    //            {
    //                if (m_set.emplace(*eter).second)
    //                    return true;
    //            }
    //            return false;
    //        }

    //        constexpr void move_next()
    //        {
    //            if (!m_eter1 || (m_eter1 && !move_next(m_eter1)))
    //                move_next(m_eter2);
    //        }

    //    public:
    //        constexpr union_set_enumerator(Eter1&& eter1, Eter2&& eter2) : m_eter1(std::forward<Eter1>(eter1)), m_eter2(std::forward<Eter2>(eter2))
    //        {
    //            move_next();
    //        }

    //        constexpr operator bool() const { return m_eter1 || m_eter2; }
    //        constexpr union_set_enumerator& operator++()
    //        {
    //            move_next();
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*() { return m_eter1 ? *m_eter1 : *m_eter2; }
    //    };
    //} // namespace impl

    //// Produces the set union of two enumerable.
    //template <typename Comparer = std::less<void>, typename E2>
    //constexpr auto union_set(E2&& e2)
    //{
    //    return [&](auto e) {
    //        using Eter1 = decltype(e.enumerator());
    //        using Eter2 = decltype(get_enumerator(std::forward<E2>(e2)));
    //        using T = remove_cref<decltype(*e.enumerator())>;
    //        return enumerable(impl::union_set_enumerator<Eter1, Eter2, T, Comparer>(e.enumerator(), get_enumerator(std::forward<E2>(e2))));
    //    };
    //}

    //namespace impl
    //{
    //    template <typename Eter1, typename T, typename Comparer>
    //    class intersect_enumerator
    //    {
    //    private:
    //        std::set<T, Comparer> m_set;
    //        Eter1 m_eter1;

    //        constexpr void move_next()
    //        {
    //            for (; m_eter1; ++m_eter1)
    //            {
    //                if (m_set.erase(*m_eter1))
    //                    break;
    //            }
    //        }

    //    public:
    //        template <typename Eter2>
    //        constexpr intersect_enumerator(Eter1&& eter1, Eter2&& eter2) : m_eter1(std::forward<Eter1>(eter1))
    //        {
    //            for (; eter2; ++eter2)
    //            {
    //                m_set.emplace(*eter2);
    //            }
    //            move_next();
    //        }

    //        constexpr operator bool() const { return m_eter1; }
    //        constexpr intersect_enumerator& operator++()
    //        {
    //            move_next();
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*() { return *m_eter1; }
    //    };
    //} // namespace impl

    //// Produces the set intersection of two enumerable.
    //template <typename Comparer = std::less<void>, typename E2>
    //constexpr auto intersect(E2&& e2)
    //{
    //    return [&](auto e) {
    //        using Eter1 = decltype(e.enumerator());
    //        using T = remove_cref<decltype(*e.enumerator())>;
    //        return enumerable(impl::intersect_enumerator<Eter1, T, Comparer>(e.enumerator(), get_enumerator(std::forward<E2>(e2))));
    //    };
    //}

    //namespace impl
    //{
    //    template <typename Eter1, typename T, typename Comparer>
    //    class except_enumerator
    //    {
    //    private:
    //        std::set<T, Comparer> m_set;
    //        Eter1 m_eter1;

    //        constexpr void move_next()
    //        {
    //            for (; m_eter1; ++m_eter1)
    //            {
    //                if (m_set.emplace(*m_eter1).second)
    //                    break;
    //            }
    //        }

    //    public:
    //        template <typename Eter2>
    //        constexpr except_enumerator(Eter1&& eter1, Eter2&& eter2) : m_eter1(std::forward<Eter1>(eter1))
    //        {
    //            for (; eter2; ++eter2)
    //            {
    //                m_set.emplace(*eter2);
    //            }
    //            move_next();
    //        }

    //        constexpr operator bool() const { return m_eter1; }
    //        constexpr except_enumerator& operator++()
    //        {
    //            move_next();
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*() { return *m_eter1; }
    //    };
    //} // namespace impl

    //// Produces the set difference of two enumerable.
    //template <typename Comparer = std::less<void>, typename E2>
    //constexpr auto except(E2&& e2)
    //{
    //    return [&](auto e) {
    //        using Eter1 = decltype(e.enumerator());
    //        using T = remove_cref<decltype(*e.enumerator())>;
    //        return enumerable(impl::except_enumerator<Eter1, T, Comparer>(e.enumerator(), get_enumerator(std::forward<E2>(e2))));
    //    };
    //}

    //namespace impl
    //{
    //    template <typename TKey, typename TElement, typename ResultSelector, typename Comparer>
    //    class group_enumerator
    //    {
    //    private:
    //        std::map<TKey, std::deque<TElement>, Comparer> m_lookup;
    //        typename std::map<TKey, std::deque<TElement>, Comparer>::iterator m_begin, m_end;
    //        ResultSelector m_rstsel;

    //    public:
    //        constexpr group_enumerator(const group_enumerator& eter) : m_lookup(eter.m_lookup), m_begin(m_lookup.begin()), m_end(m_lookup.end()), m_rstsel(eter.m_rstsel) {}
    //        constexpr group_enumerator& operator=(const group_enumerator& eter)
    //        {
    //            m_lookup = eter.m_lookup;
    //            m_begin = m_lookup.begin();
    //            m_end = m_lookup.end();
    //            m_rstsel = eter.m_rstsel;
    //            return *this;
    //        }

    //        template <typename Eter, typename KeySelector, typename ElementSelector>
    //        constexpr group_enumerator(Eter&& eter, KeySelector&& keysel, ElementSelector&& elesel, ResultSelector&& rstsel) : m_rstsel(rstsel)
    //        {
    //            for (; eter; ++eter)
    //            {
    //                m_lookup[keysel(*eter)].emplace_back(elesel(*eter));
    //            }
    //            m_begin = m_lookup.begin();
    //            m_end = m_lookup.end();
    //        }

    //        constexpr operator bool() const { return m_begin != m_end; }
    //        constexpr group_enumerator& operator++()
    //        {
    //            ++m_begin;
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*()
    //        {
    //            auto&& f{ m_begin->first };
    //            return m_rstsel(f, get_enumerable(m_lookup[f]));
    //        }
    //    };
    //} // namespace impl

    //// Groups the elements according to a specified key selector function and creates a result value from each group and its key.
    //// Key values are compared by using a specified comparer, and the elements of each group are projected by using a specified function.
    //template <typename Comparer = std::less<void>, typename KeySelector, typename ElementSelector, typename ResultSelector>
    //constexpr auto group(KeySelector&& keysel, ElementSelector&& elesel, ResultSelector&& rstsel)
    //{
    //    return [&](auto e) {
    //        using TKey = remove_cref<decltype(keysel(*e.enumerator()))>;
    //        using TElement = remove_cref<decltype(elesel(*e.enumerator()))>;
    //        return enumerable(impl::group_enumerator<TKey, TElement, ResultSelector, Comparer>(e.enumerator(), std::forward<KeySelector>(keysel), std::forward<ElementSelector>(elesel), std::forward<ResultSelector>(rstsel)));
    //    };
    //}

    //namespace impl
    //{
    //    template <typename Eter, typename TKey, typename TElement, typename KeySelector, typename ResultSelector, typename Comparer>
    //    class group_join_enumerator
    //    {
    //    private:
    //        Eter m_eter;
    //        std::map<TKey, std::deque<TElement>, Comparer> m_lookup;
    //        KeySelector m_keysel;
    //        ResultSelector m_rstsel;

    //    public:
    //        template <typename Eter2, typename KeySelector2, typename ElementSelector2>
    //        constexpr group_join_enumerator(Eter&& eter, Eter2&& eter2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
    //            : m_eter(std::forward<Eter>(eter)), m_keysel(keysel), m_rstsel(rstsel)
    //        {
    //            for (; eter2; ++eter2)
    //            {
    //                m_lookup[keysel2(*eter2)].emplace_back(elesel2(*eter2));
    //            }
    //        }

    //        constexpr operator bool() const { return m_eter; }
    //        constexpr group_join_enumerator& operator++()
    //        {
    //            ++m_eter;
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*() { return m_rstsel(*m_eter, get_enumerable(m_lookup[m_keysel(*m_eter)])); }
    //    };
    //} // namespace impl

    //// Correlates the elements of two enumerable based on key comparer and groups the results.
    //template <typename Comparer = std::less<void>, typename E2, typename KeySelector, typename KeySelector2, typename ElementSelector2, typename ResultSelector>
    //constexpr auto group_join(E2&& e2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
    //{
    //    return [&](auto e) {
    //        using Eter = decltype(e.enumerator());
    //        using TKey = remove_cref<decltype(keysel2(*get_enumerator(e2)))>;
    //        using TElement = remove_cref<decltype(elesel2(*get_enumerator(e2)))>;
    //        return enumerable(impl::group_join_enumerator<Eter, TKey, TElement, KeySelector, ResultSelector, Comparer>(e.enumerator(), get_enumerator(std::forward<E2>(e2)), std::forward<KeySelector>(keysel), std::forward<KeySelector2>(keysel2), std::forward<ElementSelector2>(elesel2), std::forward<ResultSelector>(rstsel)));
    //    };
    //}

    //namespace impl
    //{
    //    template <typename Eter, typename TKey, typename TElement, typename KeySelector, typename ResultSelector, typename Comparer>
    //    class join_enumerator
    //    {
    //    private:
    //        Eter m_eter;
    //        std::map<TKey, std::deque<TElement>, Comparer> m_lookup;
    //        typename std::deque<TElement>::iterator m_begin, m_end;
    //        KeySelector m_keysel;
    //        ResultSelector m_rstsel;

    //        constexpr void move_next()
    //        {
    //            if (m_eter)
    //            {
    //                decltype(auto) deq = m_lookup[m_keysel(*m_eter)];
    //                m_begin = deq.begin();
    //                m_end = deq.end();
    //            }
    //        }

    //    public:
    //        template <typename Eter2, typename KeySelector2, typename ElementSelector2>
    //        constexpr join_enumerator(Eter&& eter, Eter2&& eter2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
    //            : m_eter(std::forward<Eter>(eter)), m_keysel(keysel), m_rstsel(rstsel)
    //        {
    //            for (; eter2; ++eter2)
    //            {
    //                m_lookup[keysel2(*eter2)].emplace_back(elesel2(*eter2));
    //            }
    //            move_next();
    //        }

    //        constexpr operator bool() const { return m_eter || m_begin != m_end; }
    //        constexpr join_enumerator& operator++()
    //        {
    //            ++m_begin;
    //            if (m_begin == m_end)
    //            {
    //                ++m_eter;
    //                move_next();
    //            }
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*() { return m_rstsel(*m_eter, *m_begin); }
    //    };
    //} // namespace impl

    //// Correlates the elements of two enumerable based on matching keys.
    //template <typename Comparer = std::less<void>, typename E2, typename KeySelector, typename KeySelector2, typename ElementSelector2, typename ResultSelector>
    //constexpr auto join(E2&& e2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
    //{
    //    return [&](auto e) {
    //        using Eter = decltype(e.enumerator());
    //        using TKey = remove_cref<decltype(keysel2(*get_enumerator(e2)))>;
    //        using TElement = remove_cref<decltype(elesel2(*get_enumerator(e2)))>;
    //        return enumerable(impl::join_enumerator<Eter, TKey, TElement, KeySelector, ResultSelector, Comparer>(e.enumerator(), get_enumerator(std::forward<E2>(e2)), std::forward<KeySelector>(keysel), std::forward<KeySelector2>(keysel2), std::forward<ElementSelector2>(elesel2), std::forward<ResultSelector>(rstsel)));
    //    };
    //}
} // namespace linq

#endif // !LINQ_AGGREGATE_HPP
