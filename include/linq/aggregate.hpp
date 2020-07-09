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

            peek_iterator_impl(It begin, It end, Action&& action)
                : m_begin(begin), m_end(end), m_action(std::forward<Action>(action)) { set_result(); }

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
            return impl::iterable{ impl::peek_iterator<It, Action>{
                impl::iterator_ctor, std::begin(container), std::end(container), std::forward<Action>(action) } };
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

            peek_index_iterator_impl(It begin, It end, Action&& action)
                : m_begin(begin), m_end(end), m_action(std::forward<Action>(action)) { set_result(); }

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
            return impl::iterable{ impl::peek_index_iterator<It, Action>{
                impl::iterator_ctor, std::begin(container), std::end(container), std::forward<Action>(action) } };
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
            return impl::iterable{ impl::default_if_empty_iterator<It, T>{
                impl::iterator_ctor, std::begin(container), std::end(container), T{} } };
        };
    }

    // Returns the elements of the specified enumerable or the specified value in a singleton collection if the enumerable is empty.
    template <typename T>
    constexpr auto default_if_empty(T&& def)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::default_if_empty_iterator<It, T>{
                impl::iterator_ctor, std::begin(container), std::end(container), std::forward<T>(def) } };
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
            using traits_type = std::iterator_traits<typename std::vector<T>::const_reverse_iterator>;

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
            std::common_type_t<T, typename std::iterator_traits<It>::value_type> result{ std::forward<T>(def) };
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

    namespace impl
    {
        template <typename It1, typename It2, typename Comparer>
        class union_set_iterator_impl
        {
        private:
            using value_type = std::common_type_t<typename std::iterator_traits<It1>::value_type, typename std::iterator_traits<It2>::value_type>;

            std::set<value_type, Comparer> m_set;
            It1 m_begin1, m_end1;
            It2 m_begin2, m_end2;

            template <typename It>
            bool move_next(It& begin, const It& end)
            {
                for (; begin != end; ++begin)
                {
                    if (m_set.emplace(*begin).second)
                        return true;
                }
                return false;
            }

        public:
            using traits_type = iterator_impl_traits<value_type>;

            union_set_iterator_impl(It1 begin1, It1 end1, It2 begin2, It2 end2) : m_begin1(begin1), m_end1(end1), m_begin2(begin2), m_end2(end2)
            {
                move_next();
            }

            typename traits_type::reference value() { return m_begin1 != m_end1 ? *m_begin1 : *m_begin2; }

            void move_next()
            {
                if (m_begin1 == m_end1 || (m_begin1 != m_end1 && !move_next(m_begin1, m_end1)))
                    move_next(m_begin2, m_end2);
            }

            bool is_valid() const { return m_begin1 != m_end1 || m_begin2 != m_end2; }
        };

        template <typename It1, typename It2, typename Comparer>
        using union_set_iterator = iterator_base<union_set_iterator_impl<It1, It2, Comparer>>;
    } // namespace impl

    // Produces the set union of two enumerable.
    template <typename Comparer = std::less<void>, typename Container2>
    constexpr auto union_set(Container2&& c2)
    {
        return [&](auto&& container) {
            using It1 = decltype(std::begin(container));
            using It2 = decltype(std::begin(c2));
            return impl::iterable{ impl::union_set_iterator<It1, It2, Comparer>{
                impl::iterator_ctor, std::begin(container), std::end(container), std::begin(c2), std::end(c2) } };
        };
    }

    namespace impl
    {
        template <typename It, typename Comparer>
        class intersect_iterator_impl
        {
        private:
            using value_type = typename std::iterator_traits<It>::value_type;

            std::set<value_type, Comparer> m_set;
            It m_begin, m_end;

        public:
            using traits_type = std::iterator_traits<It>;

            template <typename It2>
            intersect_iterator_impl(It begin, It end, It2 begin2, It2 end2) : m_set(begin2, end2), m_begin(begin), m_end(end)
            {
                move_next();
            }

            typename traits_type::reference value() { return *m_begin; }

            void move_next()
            {
                for (; m_begin != m_end; ++m_begin)
                {
                    if (m_set.erase(*m_begin))
                        break;
                }
            }

            bool is_valid() const { return m_begin != m_end; }
        };

        template <typename It, typename Comparer>
        using intersect_iterator = iterator_base<intersect_iterator_impl<It, Comparer>>;
    } // namespace impl

    // Produces the set intersection of two enumerable.
    template <typename Comparer = std::less<void>, typename C2>
    constexpr auto intersect(C2&& c2)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::intersect_iterator<It, Comparer>{
                impl::iterator_ctor, std::begin(container), std::end(container), std::begin(c2), std::end(c2) } };
        };
    }

    namespace impl
    {
        template <typename It, typename Comparer>
        class except_iterator_impl
        {
        private:
            using value_type = typename std::iterator_traits<It>::value_type;

            std::set<value_type, Comparer> m_set;
            It m_begin, m_end;

        public:
            using traits_type = std::iterator_traits<It>;

            template <typename It2>
            except_iterator_impl(It begin, It end, It2 begin2, It2 end2) : m_set(begin2, end2), m_begin(begin), m_end(end)
            {
                move_next();
            }

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
        using except_iterator = iterator_base<except_iterator_impl<It, Comparer>>;
    } // namespace impl

    // Produces the set difference of two enumerable.
    template <typename Comparer = std::less<void>, typename C2>
    constexpr auto except(C2&& c2)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::except_iterator<It, Comparer>{ impl::iterator_ctor, std::begin(container), std::end(container), std::begin(c2), std::end(c2) } };
        };
    }

    namespace impl
    {
        template <typename TKey, typename TElement, typename ResultSelector, typename Comparer>
        class group_iterator_impl
        {
        private:
            std::map<TKey, std::vector<TElement>, Comparer> m_lookup;
            typename std::map<TKey, std::vector<TElement>, Comparer>::iterator m_begin, m_end;
            std::decay_t<ResultSelector> m_rstsel;

            using result_type = decltype(m_rstsel(m_begin->first, m_lookup[m_begin->first]));
            std::optional<result_type> m_result{};

            void set_result()
            {
                if (m_begin != m_end)
                {
                    auto& f = m_begin->first;
                    m_result = m_rstsel(f, m_lookup[f]);
                }
            }

        public:
            using traits_type = iterator_impl_traits<result_type>;

            template <typename It, typename KeySelector, typename ElementSelector>
            group_iterator_impl(It begin, It end, KeySelector&& keysel, ElementSelector&& elesel, ResultSelector&& rstsel) : m_rstsel(std::forward<ResultSelector>(rstsel))
            {
                for (; begin != end; ++begin)
                {
                    m_lookup[keysel(*begin)].emplace_back(elesel(*begin));
                }
                m_begin = m_lookup.begin();
                m_end = m_lookup.end();
                set_result();
            }

            typename traits_type::reference value() { return *m_result; }

            void move_next()
            {
                ++m_begin;
                set_result();
            }

            bool is_valid() const { return m_begin != m_end; }
        };

        template <typename TKey, typename TElement, typename ResultSelector, typename Comparer>
        using group_iterator = iterator_base<group_iterator_impl<TKey, TElement, ResultSelector, Comparer>>;
    } // namespace impl

    // Groups the elements according to a specified key selector function and creates a result value from each group and its key.
    // Key values are compared by using a specified comparer, and the elements of each group are projected by using a specified function.
    template <typename Comparer = std::less<void>, typename KeySelector, typename ElementSelector, typename ResultSelector>
    constexpr auto group(KeySelector&& keysel, ElementSelector&& elesel, ResultSelector&& rstsel)
    {
        return [&](auto&& container) {
            using TKey = std::remove_reference_t<decltype(keysel(*std::begin(container)))>;
            using TElement = std::remove_reference_t<decltype(elesel(*std::begin(container)))>;
            return impl::iterable{ impl::group_iterator<TKey, TElement, ResultSelector, Comparer>{
                impl::iterator_ctor,
                std::begin(container), std::end(container),
                std::forward<KeySelector>(keysel), std::forward<ElementSelector>(elesel), std::forward<ResultSelector>(rstsel) } };
        };
    }

    namespace impl
    {
        template <typename It, typename TKey, typename TElement, typename KeySelector, typename ResultSelector, typename Comparer>
        class group_join_iterator_impl
        {
        private:
            It m_begin, m_end;
            std::map<TKey, std::vector<TElement>, Comparer> m_lookup;
            KeySelector m_keysel;
            ResultSelector m_rstsel;

            using result_type = decltype(m_rstsel(*m_begin, m_lookup[m_keysel(*m_begin)]));
            std::optional<result_type> m_result;

            void set_result()
            {
                if (m_begin != m_end)
                {
                    auto& item = *m_begin;
                    m_result = m_rstsel(item, m_lookup[m_keysel(item)]);
                }
            }

        public:
            using traits_type = iterator_impl_traits<result_type>;

            template <typename It2, typename KeySelector2, typename ElementSelector2>
            group_join_iterator_impl(It begin, It end, It2 begin2, It2 end2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
                : m_begin(begin), m_end(end), m_keysel(std::forward<KeySelector>(keysel)), m_rstsel(std::forward<ResultSelector>(rstsel))
            {
                for (; begin2 != end2; ++begin2)
                {
                    m_lookup[keysel2(*begin2)].emplace_back(elesel2(*begin2));
                }
                set_result();
            }

            typename traits_type::reference value() { return *m_result; }

            void move_next()
            {
                ++m_begin;
                set_result();
            }

            bool is_valid() const { return m_begin != m_end; }
        };

        template <typename It, typename TKey, typename TElement, typename KeySelector, typename ResultSelector, typename Comparer>
        using group_join_iterator = iterator_base<group_join_iterator_impl<It, TKey, TElement, KeySelector, ResultSelector, Comparer>>;
    } // namespace impl

    // Correlates the elements of two enumerable based on key comparer and groups the results.
    template <typename Comparer = std::less<void>, typename C2, typename KeySelector, typename KeySelector2, typename ElementSelector2, typename ResultSelector>
    constexpr auto group_join(C2&& c2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            using TKey = std::remove_reference_t<decltype(keysel2(*std::begin(c2)))>;
            using TElement = std::remove_reference_t<decltype(elesel2(*std::begin(c2)))>;
            return impl::iterable{ impl::group_join_iterator<It, TKey, TElement, KeySelector, ResultSelector, Comparer>{
                impl::iterator_ctor, std::begin(container), std::end(container),
                std::begin(c2), std::end(c2),
                std::forward<KeySelector>(keysel), std::forward<KeySelector2>(keysel2), std::forward<ElementSelector2>(elesel2),
                std::forward<ResultSelector>(rstsel) } };
        };
    }

    namespace impl
    {
        template <typename It, typename TKey, typename TElement, typename KeySelector, typename ResultSelector, typename Comparer>
        class join_iterator_impl
        {
        private:
            It m_begin, m_end;
            std::map<TKey, std::vector<TElement>, Comparer> m_lookup;
            typename std::vector<TElement>::iterator m_inner_begin, m_inner_end;
            KeySelector m_keysel;
            ResultSelector m_rstsel;

            using result_type = decltype(m_rstsel(*m_begin, *m_inner_begin));
            std::optional<result_type> m_result;

            void set_result()
            {
                if (m_begin != m_end)
                {
                    auto& vec = m_lookup[m_keysel(*m_begin)];
                    m_inner_begin = vec.begin();
                    m_inner_end = vec.end();
                    m_result = m_rstsel(*m_begin, *m_inner_begin);
                }
            }

        public:
            using traits_type = iterator_impl_traits<result_type>;

            template <typename It2, typename KeySelector2, typename ElementSelector2>
            join_iterator_impl(It begin, It end, It2 begin2, It2 end2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
                : m_begin(begin), m_end(end), m_keysel(std::forward<KeySelector>(keysel)), m_rstsel(std::forward<ResultSelector>(rstsel))
            {
                for (; begin2 != end2; ++begin2)
                {
                    m_lookup[keysel2(*begin2)].emplace_back(elesel2(*begin2));
                }
                set_result();
            }

            typename traits_type::reference value() { return *m_result; }

            void move_next()
            {
                ++m_inner_begin;
                if (m_inner_begin == m_inner_end)
                {
                    ++m_begin;
                    set_result();
                }
            }

            bool is_valid() const { return m_begin != m_end || m_inner_begin != m_inner_end; }
        };

        template <typename It, typename TKey, typename TElement, typename KeySelector, typename ResultSelector, typename Comparer>
        using join_iterator = iterator_base<join_iterator_impl<It, TKey, TElement, KeySelector, ResultSelector, Comparer>>;
    } // namespace impl

    // Correlates the elements of two enumerable based on matching keys.
    template <typename Comparer = std::less<void>, typename C2, typename KeySelector, typename KeySelector2, typename ElementSelector2, typename ResultSelector>
    constexpr auto join(C2&& c2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            using TKey = std::remove_reference_t<decltype(keysel2(*std::begin(c2)))>;
            using TElement = std::remove_reference_t<decltype(elesel2(*std::begin(c2)))>;
            return impl::iterable{ impl::join_iterator<It, TKey, TElement, KeySelector, ResultSelector, Comparer>{
                impl::iterator_ctor, std::begin(container), std::end(container),
                std::begin(c2), std::end(c2),
                std::forward<KeySelector>(keysel), std::forward<KeySelector2>(keysel2), std::forward<ElementSelector2>(elesel2),
                std::forward<ResultSelector>(rstsel) } };
        };
    }
} // namespace linq

#endif // !LINQ_AGGREGATE_HPP
