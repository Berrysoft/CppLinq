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
        return [=]<impl::container Container>(Container container) mutable {
            for (auto& item : container)
                action(item);
            return container;
        };
    }

    // Applies an action to each element and return the enumerable.
    template <typename Action>
    constexpr auto for_each_index(Action&& action)
    {
        return [=]<impl::container Container>(Container container) mutable {
            std::size_t i = 0;
            for (auto& item : container)
            {
                action(item, i);
                i++;
            }
            return container;
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
        return [=]<impl::container Container>(Container container) mutable {
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
        return [=]<impl::container Container>(Container container) mutable {
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
        return [=]<impl::container Container>(Container container) mutable {
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
        return []<impl::container Container>(Container container) {
            auto begin = std::begin(container);
            auto end = std::end(container);
            return begin == end;
        };
    }

    template <typename T, typename Comparer = std::equal_to<void>>
    constexpr auto contains(T&& value, Comparer&& comparer = {})
    {
        return any([=](auto&& a) mutable { return comparer(a, value); });
    }

    // Applies an accumulator function over an enumerable.
    template <typename T, typename Func>
    constexpr auto aggregate(T&& seed, Func&& func)
    {
        return [=]<impl::container Container>(Container container) mutable {
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
        return [=]<impl::container Container>(Container container) mutable {
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
        return [=]<impl::container Container>(Container container) mutable {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            return front<Pred, T>(std::forward<Pred>(pred))(container);
        };
    }

    // Returns the last element that satisfies a condition or a default value if no such element is found.
    template <typename Pred = always_true, typename T>
    constexpr auto back(Pred&& pred = {}, T&& def = {})
    {
        return [=]<impl::container Container>(Container container) mutable {
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
        return [=]<impl::container Container>(Container container) mutable {
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
        return [=]<impl::container Container>(Container container) mutable {
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
        return [=]<impl::container Container>(Container container) mutable {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            return single<Pred, T>(std::forward<Pred>(pred))(container);
        };
    }

    // Returns the elements of the specified enumerable or the specified value in a singleton collection if the enumerable is empty.
    template <typename T>
    constexpr auto default_if_empty(T&& def)
    {
        return [=]<impl::container Container>(Container container)
                   -> generator<std::remove_cvref_t<std::common_type_t<typename std::iterator_traits<decltype(std::begin(container))>::value_type, T>>> {
            auto begin = std::begin(container);
            auto end = std::end(container);
            if (begin == end) { co_yield def; }
            else
            {
                for (; begin != end; ++begin) co_yield* begin;
            }
        };
    }

    // Returns the elements of the specified enumerable or the element type's default value in a singleton collection if the enumerable is empty.
    constexpr auto default_if_empty()
    {
        return []<impl::container Container>(Container container)
                   -> generator<typename std::iterator_traits<decltype(std::begin(container))>::value_type> {
            auto begin = std::begin(container);
            auto end = std::end(container);
            if (begin == end) { co_yield {}; }
            else
            {
                for (; begin != end; ++begin) co_yield* begin;
            }
        };
    }

    // Calculates the average value of the elements.
    constexpr auto average()
    {
        return []<impl::container Container>(Container container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            std::size_t num{ 0 };
            return aggregate<T>({}, [&num](auto& a, auto& b) { ++num; return a + b; })(container) / static_cast<T>(num);
        };
    }

    // Calculates the sum of the elements.
    constexpr auto sum()
    {
        return []<impl::container Container>(Container container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            return aggregate<T>({}, [](auto& a, auto& b) { return a + b; })(container);
        };
    }

    namespace impl
    {
        template <typename T>
        struct is_wrapped_reversible_container : std::false_type
        {
        };

        template <reversible_container T>
        struct is_wrapped_reversible_container<container_view<T>> : std::true_type
        {
        };

        template <typename T>
        inline constexpr bool is_wrapped_reversible_container_v = is_wrapped_reversible_container<T>::value;
    } // namespace impl

    // Inverts the order of the elements.
    constexpr auto reverse()
    {
        return []<impl::container Container>(Container container)
                   -> generator<typename std::iterator_traits<decltype(std::begin(container))>::value_type> {
            if constexpr (impl::is_reversible_container_v<Container>)
            {
                auto end = std::rend(container);
                for (auto it = std::rbegin(container); it != end; ++it)
                {
                    co_yield* it;
                }
            }
            else if constexpr (impl::is_wrapped_reversible_container_v<Container>)
            {
                auto end = std::rend(container.base());
                for (auto it = std::rbegin(container.base()); it != end; ++it)
                {
                    co_yield* it;
                }
            }
            else
            {
                std::vector<typename std::iterator_traits<decltype(std::begin(container))>::value_type> vec(std::begin(container), std::end(container));
                auto end = vec.rend();
                for (auto it = vec.rbegin(); it != end; ++it)
                {
                    co_yield* it;
                }
            }
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
        return [=](auto&& t1, auto&& t2) mutable {
            return comparer(selector(t1), selector(t2));
        };
    }

    namespace impl
    {
        template <typename C1, typename... Comparer>
        constexpr auto consume_comparer(C1&& c1, Comparer&&... comparer)
        {
            return [=](auto&& t1, auto&& t2) mutable {
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
        return [=]<impl::container Container>(Container container) mutable {
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
        return [=]<impl::container Container>(Container container) mutable {
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
        return [=]<impl::container Container>(Container container) mutable {
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
        return [=, &def]<impl::container Container>(Container container) {
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
        return [=]<impl::container Container>(Container container) {
            using It = decltype(std::begin(container));
            using T = typename std::iterator_traits<It>::value_type;
            return get_at<T>(index, {})(container);
        };
    }

    // Returns the index of the first element which satifies the predicate.
    template <typename Pred>
    constexpr auto index_of(Pred&& pred)
    {
        return [=]<impl::container Container>(Container container) mutable {
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

    // Returns distinct elements.
    template <typename Comparer = std::less<void>>
    constexpr auto distinct()
    {
        return []<impl::container Container>(Container container)
                   -> generator<typename std::iterator_traits<decltype(std::begin(container))>::value_type> {
            std::set<typename std::iterator_traits<decltype(std::begin(container))>::value_type, Comparer> set;
            for (auto&& item : container)
            {
                if (set.emplace(item).second) co_yield item;
            }
        };
    }

    // Produces the set union of two enumerable.
    template <typename Comparer = std::less<void>, typename Container2>
    constexpr auto union_set(Container2&& c2)
    {
        return [c2 = impl::decay_container(std::forward<Container2>(c2))]<impl::container Container>(Container container)
                   -> generator<std::remove_cvref_t<std::common_type_t<
                       typename std::iterator_traits<decltype(std::begin(container))>::value_type,
                       typename std::iterator_traits<decltype(std::begin(c2))>::value_type>>> {
            std::set<typename std::iterator_traits<decltype(std::begin(container))>::value_type, Comparer> set;
            for (auto&& item : container)
            {
                if (set.emplace(item).second) co_yield item;
            }
            for (auto&& item : c2)
            {
                if (set.emplace(item).second) co_yield item;
            }
        };
    }

    // Produces the set intersection of two enumerable.
    template <typename Comparer = std::less<void>, typename C2>
    constexpr auto intersect(C2&& c2)
    {
        return [c2 = impl::decay_container(std::forward<C2>(c2))]<impl::container Container>(Container container)
                   -> generator<std::remove_cvref_t<std::common_type_t<
                       typename std::iterator_traits<decltype(std::begin(container))>::value_type,
                       typename std::iterator_traits<decltype(std::begin(c2))>::value_type>>> {
            std::set<typename std::iterator_traits<decltype(std::begin(container))>::value_type, Comparer> set(std::begin(container), std::end(container));
            for (auto&& item : c2)
            {
                if (set.erase(item)) co_yield item;
            }
        };
    }

    // Produces the set difference of two enumerable.
    template <typename Comparer = std::less<void>, typename C2>
    constexpr auto except(C2&& c2)
    {
        return [c2 = impl::decay_container(std::forward<C2>(c2))]<impl::container Container>(Container container)
                   -> generator<std::remove_cvref_t<std::common_type_t<
                       typename std::iterator_traits<decltype(std::begin(container))>::value_type,
                       typename std::iterator_traits<decltype(std::begin(c2))>::value_type>>> {
            std::set<typename std::iterator_traits<decltype(std::begin(c2))>::value_type, Comparer> set(std::begin(c2), std::end(c2));
            for (auto&& item : container)
            {
                if (set.emplace(item).second) co_yield item;
            }
        };
    }

    // Groups the elements according to a specified key selector function and creates a result value from each group and its key.
    // Key values are compared by using a specified comparer, and the elements of each group are projected by using a specified function.
    template <typename Comparer = std::less<void>, typename KeySelector, typename ElementSelector, typename ResultSelector>
    constexpr auto group(KeySelector&& keysel, ElementSelector&& elesel, ResultSelector&& rstsel)
    {
        return [=]<impl::container Container>(Container container) mutable
               -> generator<std::remove_cvref_t<decltype(rstsel(keysel(*std::begin(container)), std::vector<std::remove_cvref_t<decltype(elesel(*std::begin(container)))>>{}))>> {
            using TKey = std::remove_cvref_t<decltype(keysel(*std::begin(container)))>;
            using TElement = std::remove_cvref_t<decltype(elesel(*std::begin(container)))>;
            std::map<TKey, std::vector<TElement>, Comparer> lookup;
            for (auto&& item : container)
            {
                lookup[keysel(item)].emplace_back(elesel(item));
            }
            for (auto&& pair : lookup)
            {
                co_yield rstsel(pair.first, pair.second);
            }
        };
    }

    // Correlates the elements of two enumerable based on key comparer and groups the results.
    template <typename Comparer = std::less<void>, typename C2, typename KeySelector, typename KeySelector2, typename ElementSelector2, typename ResultSelector>
    constexpr auto group_join(C2&& c2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
    {
        return [=, c2 = impl::decay_container(std::forward<C2>(c2))]<impl::container Container>(Container container) mutable
               -> generator<std::remove_cvref_t<decltype(rstsel(*std::begin(container), std::vector<std::remove_cvref_t<decltype(elesel2(*std::begin(c2)))>>{}))>> {
            using TKey = std::remove_cvref_t<decltype(keysel2(*std::begin(c2)))>;
            using TElement = std::remove_cvref_t<decltype(elesel2(*std::begin(c2)))>;
            std::map<TKey, std::vector<TElement>, Comparer> lookup;
            for (auto&& item : c2)
            {
                lookup[keysel2(item)].emplace_back(elesel2(item));
            }
            for (auto&& item : container)
            {
                co_yield rstsel(item, lookup[keysel(item)]);
            }
        };
    }

    // Correlates the elements of two enumerable based on matching keys.
    template <typename Comparer = std::less<void>, typename C2, typename KeySelector, typename KeySelector2, typename ElementSelector2, typename ResultSelector>
    constexpr auto join(C2&& c2, KeySelector&& keysel, KeySelector2&& keysel2, ElementSelector2&& elesel2, ResultSelector&& rstsel)
    {
        return [=, c2 = impl::decay_container(std::forward<C2>(c2))]<impl::container Container>(Container container) mutable
               -> generator<std::remove_cvref_t<decltype(rstsel(*std::begin(container), std::declval<std::remove_cvref_t<decltype(elesel2(*std::begin(c2)))>>()))>> {
            using TKey = std::remove_cvref_t<decltype(keysel2(*std::begin(c2)))>;
            using TElement = std::remove_cvref_t<decltype(elesel2(*std::begin(c2)))>;
            std::map<TKey, std::vector<TElement>, Comparer> lookup;
            for (auto&& item : c2)
            {
                lookup[keysel2(item)].emplace_back(elesel2(item));
            }
            for (auto&& item : container)
            {
                for (auto&& item2 : lookup[keysel(item)])
                {
                    co_yield rstsel(item, item2);
                }
            }
        };
    }
} // namespace linq

#endif // !LINQ_AGGREGATE_HPP
