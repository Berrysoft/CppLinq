/**CppLinq query.hpp
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
#ifndef LINQ_QUERY_HPP
#define LINQ_QUERY_HPP

#include <linq/core.hpp>
#include <tuple>

namespace linq
{
    // Filters an enumerable based on a predicate.
    template <typename Pred>
    constexpr auto where(Pred&& pred)
    {
        return [pred = impl::decay(std::forward<Pred>(pred))]<impl::container Container>(Container container)
                   -> generator<std::remove_reference_t<decltype(*std::begin(container))>> {
            for (auto&& item : container)
            {
                if (pred(item)) co_yield item;
            }
        };
    }

    // Filters an enumerable based on a predicate.
    template <typename Pred>
    constexpr auto where_index(Pred&& pred)
    {
        return [pred = impl::decay(std::forward<Pred>(pred))]<impl::container Container>(Container container)
                   -> generator<std::remove_reference_t<decltype(*std::begin(container))>> {
            std::size_t index{ 0 };
            for (auto&& item : container)
            {
                if (pred(item, index)) co_yield item;
                index++;
            }
        };
    }

    // Projects each element into a new form.
    template <typename Selector>
    constexpr auto select(Selector&& selector)
    {
        return [selector = impl::decay(std::forward<Selector>(selector))]<impl::container Container>(Container container)
                   -> generator<std::remove_reference_t<decltype(selector(*std::begin(container)))>> {
            for (auto&& item : container)
            {
                co_yield selector(item);
            }
        };
    }

    // Projects each element into a new form.
    template <typename Selector>
    constexpr auto select_index(Selector&& selector)
    {
        return [selector = impl::decay(std::forward<Selector>(selector))]<impl::container Container>(Container container)
                   -> generator<std::remove_reference_t<decltype(selector(*std::begin(container), std::size_t{}))>> {
            std::size_t index{ 0 };
            for (auto&& item : container)
            {
                co_yield selector(item, index);
                index++;
            }
        };
    }

    // Projects each element, flattens the resulting sequences into one sequence, and invokes a result selector function on each element therein.
    template <typename CSelector, typename RSelector>
    constexpr auto select_many(CSelector&& cselector, RSelector&& rselector)
    {
        return [cselector = impl::decay(std::forward<CSelector>(cselector)),
                rselector = impl::decay(std::forward<RSelector>(rselector))]<impl::container Container>(Container container)
                   -> generator<std::remove_reference_t<decltype(rselector(*std::begin(container), *std::begin(cselector(*std::begin(container)))))>> {
            for (auto&& c : container)
            {
                for (auto&& item : cselector(c))
                {
                    co_yield rselector(c, item);
                }
            }
        };
    }

    // Projects each element, flattens the resulting sequences into one sequence, and invokes a result selector function on each element therein.
    template <typename CSelector, typename RSelector>
    constexpr auto select_many_index(CSelector&& cselector, RSelector&& rselector)
    {
        return [cselector = impl::decay(std::forward<CSelector>(cselector)),
                rselector = impl::decay(std::forward<RSelector>(rselector))]<impl::container Container>(Container container)
                   -> generator<std::remove_reference_t<decltype(rselector(*std::begin(container), *std::begin(cselector(*std::begin(container), std::size_t{}))))>> {
            std::size_t index{ 0 };
            for (auto&& c : container)
            {
                for (auto&& item : cselector(c, index))
                {
                    co_yield rselector(c, item);
                }
                index++;
            }
        };
    }

    // Bypasses a specified number of elements and then returns the remaining elements.
    constexpr auto skip(std::size_t skipn)
    {
        return [=]<impl::container Container>(Container container) mutable
               -> generator<std::remove_reference_t<decltype(*std::begin(container))>> {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && skipn--; ++begin)
                ;
            for (; begin != end; ++begin)
            {
                co_yield* begin;
            }
        };
    }

    // Bypasses elements as long as a specified condition is true and then returns the remaining elements.
    template <typename Pred>
    constexpr auto skip_while(Pred&& pred)
    {
        return [pred = impl::decay(std::forward<Pred>(pred))]<impl::container Container>(Container container)
                   -> generator<std::remove_reference_t<decltype(*std::begin(container))>> {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && pred(*begin); ++begin)
                ;
            for (; begin != end; ++begin)
            {
                co_yield* begin;
            }
        };
    }

    // Bypasses elements as long as a specified condition is true and then returns the remaining elements.
    template <typename Pred>
    constexpr auto skip_while_index(Pred&& pred)
    {
        return [pred = impl::decay(std::forward<Pred>(pred))]<impl::container Container>(Container container)
                   -> generator<std::remove_reference_t<decltype(*std::begin(container))>> {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (std::size_t i{ 0 }; begin != end && pred(*begin, i); ++i, ++begin)
                ;
            for (; begin != end; ++begin)
            {
                co_yield* begin;
            }
        };
    }

    // Returns a specified number of contiguous elements from the start.
    constexpr auto take(std::size_t taken)
    {
        return [=]<impl::container Container>(Container container) mutable
               -> generator<std::remove_reference_t<decltype(*std::begin(container))>> {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && taken--; ++begin)
            {
                co_yield* begin;
            }
        };
    }

    // Returns elements as long as a specified condition is true.
    template <typename Pred>
    constexpr auto take_while(Pred&& pred)
    {
        return [pred = impl::decay(std::forward<Pred>(pred))]<impl::container Container>(Container container)
                   -> generator<std::remove_reference_t<decltype(*std::begin(container))>> {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && pred(*begin); ++begin)
            {
                co_yield* begin;
            }
        };
    }

    // Returns elements as long as a specified condition is true.
    template <typename Pred>
    constexpr auto take_while_index(Pred&& pred)
    {
        return [pred = impl::decay(std::forward<Pred>(pred))]<impl::container Container>(Container container)
                   -> generator<std::remove_reference_t<decltype(*std::begin(container))>> {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (std::size_t i{ 0 }; begin != end && pred(*begin, i); ++i, ++begin)
            {
                co_yield* begin;
            }
        };
    }

    namespace impl
    {
        template <typename E1, typename... Es>
        constexpr bool and_all(E1&& e1, Es&&... es)
        {
            if constexpr (sizeof...(Es) == 0)
            {
                return e1;
            }
            else
            {
                return e1 && and_all<Es...>(std::forward<Es>(es)...);
            }
        }

        template <typename... T>
        constexpr void expand_tuple(T&&...)
        {
        }

        template <typename It1, typename It2>
        struct iterator_pack
        {
            It1 m_begin;
            It2 m_end;

            iterator_pack() {}
            iterator_pack(It1 begin, It2 end) : m_begin(begin), m_end(end) {}
        };

        template <typename It1, typename It2>
        iterator_pack(It1, It2) -> iterator_pack<It1, It2>;
    } // namespace impl

    // Applies a specified function to the corresponding elements of two enumerable, producing an enumerable of the results.
    template <typename Selector, typename... Containers>
    constexpr auto zip(Selector&& selector, Containers&&... cs)
    {
        return [selector = impl::decay(std::forward<Selector>(selector)),
                ... cs = impl::decay_container<Containers>(std::forward<Containers>(cs))]<impl::container Container>(Container container)
                   -> generator<std::remove_reference_t<decltype(selector(*std::begin(container), *std::begin(cs)...))>> {
            auto its = std::make_tuple(
                impl::iterator_pack{ std::begin(container), std::end(container) },
                impl::iterator_pack{ std::begin(cs), std::end(cs) }...);
            for (;
                 std::apply([](auto&&... pack) { return impl::and_all((pack.m_begin != pack.m_end)...); }, its);
                 std::apply([](auto&&... pack) { return impl::expand_tuple((++pack.m_begin)...); }, its))
            {
                co_yield std::apply([selector](auto&&... pack) { return selector((*pack.m_begin)...); }, its);
            }
        };
    }

    // Applies a specified function to the corresponding elements of two enumerable, producing an enumerable of the results.
    template <typename Selector, typename... Containers>
    constexpr auto zip_index(Selector&& selector, Containers&&... cs)
    {
        return [selector = impl::decay(std::forward<Selector>(selector)),
                ... cs = impl::decay_container<Containers>(std::forward<Containers>(cs))]<impl::container Container>(Container container)
                   -> generator<std::remove_reference_t<decltype(selector(*std::begin(container), *std::begin(cs)..., std::size_t{}))>> {
            auto its = std::make_tuple(
                impl::iterator_pack{ std::begin(container), std::end(container) },
                impl::iterator_pack{ std::begin(cs), std::end(cs) }...);
            for (std::size_t i{ 0 };
                 std::apply([](auto&&... pack) { return impl::and_all((pack.m_begin != pack.m_end)...); }, its);
                 ++i, std::apply([](auto&&... pack) { return impl::expand_tuple((++pack.m_begin)...); }, its))
            {
                co_yield std::apply([selector, i](auto&&... pack) { return selector((*pack.m_begin)..., i); }, its);
            }
        };
    }
} // namespace linq

#endif // !LINQ_QUERY_HPP
