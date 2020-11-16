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
    struct where
    {
        mutable Pred m_pred;

        where(Pred&& pred) : m_pred(std::forward<Pred>(pred)) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            for (auto&& item : container)
            {
                if (m_pred(item)) co_yield item;
            }
        }
    };

    template <typename Pred>
    where(Pred &&) -> where<Pred>;

    // Filters an enumerable based on a predicate.
    template <typename Pred>
    struct where_index
    {
        mutable Pred m_pred;

        where_index(Pred&& pred) : m_pred(std::forward<Pred>(pred)) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            std::size_t index{ 0 };
            for (auto&& item : container)
            {
                if (m_pred(item, index)) co_yield item;
                index++;
            }
        }
    };

    template <typename Pred>
    where_index(Pred &&) -> where_index<Pred>;

    // Projects each element into a new form.
    template <typename Selector>
    struct select
    {
        mutable Selector m_selector;

        select(Selector&& selector) : m_selector(std::forward<Selector>(selector)) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<std::remove_cvref_t<decltype(m_selector(*std::begin(container)))>>
        {
            for (auto&& item : container)
            {
                co_yield m_selector(item);
            }
        }
    };

    template <typename Selector>
    select(Selector &&) -> select<Selector>;

    // Projects each element into a new form.
    template <typename Selector>
    struct select_index
    {
        mutable Selector m_selector;

        select_index(Selector&& selector) : m_selector(std::forward<Selector>(selector)) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<std::remove_cvref_t<decltype(m_selector(*std::begin(container), std::size_t{}))>>
        {
            std::size_t index{ 0 };
            for (auto&& item : container)
            {
                co_yield m_selector(item, index);
                index++;
            }
        }
    };

    template <typename Selector>
    select_index(Selector &&) -> select_index<Selector>;

    // Projects each element, flattens the resulting sequences into one sequence, and invokes a result selector function on each element therein.
    template <typename CSelector, typename RSelector>
    struct select_many
    {
        mutable CSelector m_cselector;
        mutable RSelector m_rselector;

        select_many(CSelector&& cselector, RSelector&& rselector) : m_cselector(std::forward<CSelector>(cselector)), m_rselector(std::forward<RSelector>(rselector)) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<std::remove_cvref_t<decltype(m_rselector(*std::begin(container), *std::begin(m_cselector(*std::begin(container)))))>>
        {
            for (auto&& c : container)
            {
                for (auto&& item : m_cselector(c))
                {
                    co_yield m_rselector(c, item);
                }
            }
        }
    };

    template <typename CSelector, typename RSelector>
    select_many(CSelector&&, RSelector &&) -> select_many<CSelector, RSelector>;

    // Projects each element, flattens the resulting sequences into one sequence, and invokes a result selector function on each element therein.
    template <typename CSelector, typename RSelector>
    struct select_many_index
    {
        mutable CSelector m_cselector;
        mutable RSelector m_rselector;

        select_many_index(CSelector&& cselector, RSelector&& rselector) : m_cselector(std::forward<CSelector>(cselector)), m_rselector(std::forward<RSelector>(rselector)) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<std::remove_cvref_t<decltype(m_rselector(*std::begin(container), *std::begin(m_cselector(*std::begin(container), std::size_t{}))))>>
        {
            std::size_t index{ 0 };
            for (auto&& c : container)
            {
                for (auto&& item : m_cselector(c, index))
                {
                    co_yield m_rselector(c, item);
                }
                index++;
            }
        }
    };

    template <typename CSelector, typename RSelector>
    select_many_index(CSelector&&, RSelector &&) -> select_many_index<CSelector, RSelector>;

    // Bypasses a specified number of elements and then returns the remaining elements.
    struct skip
    {
        std::size_t m_skipn;

        constexpr skip(std::size_t skipn) noexcept : m_skipn(skipn) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            std::size_t n{ m_skipn };
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && n--; ++begin)
                ;
            for (; begin != end; ++begin)
            {
                co_yield* begin;
            }
        }
    };

    // Bypasses elements as long as a specified condition is true and then returns the remaining elements.
    template <typename Pred>
    struct skip_while
    {
        mutable Pred m_pred;

        skip_while(Pred&& pred) : m_pred(std::forward<Pred>(pred)) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && m_pred(*begin); ++begin)
                ;
            for (; begin != end; ++begin)
            {
                co_yield* begin;
            }
        }
    };

    template <typename Pred>
    skip_while(Pred &&) -> skip_while<Pred>;

    // Bypasses elements as long as a specified condition is true and then returns the remaining elements.
    template <typename Pred>
    struct skip_while_index
    {
        mutable Pred m_pred;

        skip_while_index(Pred&& pred) : m_pred(std::forward<Pred>(pred)) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (std::size_t i{ 0 }; begin != end && m_pred(*begin, i); ++i, ++begin)
                ;
            for (; begin != end; ++begin)
            {
                co_yield* begin;
            }
        }
    };

    template <typename Pred>
    skip_while_index(Pred &&) -> skip_while_index<Pred>;

    // Returns a specified number of contiguous elements from the start.
    struct take
    {
        std::size_t m_taken;

        constexpr take(std::size_t taken) noexcept : m_taken(taken) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            std::size_t n{ m_taken };
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && n--; ++begin)
            {
                co_yield* begin;
            }
        }
    };

    // Returns elements as long as a specified condition is true.
    template <typename Pred>
    struct take_while
    {
        mutable Pred m_pred;

        take_while(Pred&& pred) : m_pred(std::forward<Pred>(pred)) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && m_pred(*begin); ++begin)
            {
                co_yield* begin;
            }
        };
    };

    template <typename Pred>
    take_while(Pred &&) -> take_while<Pred>;

    // Returns elements as long as a specified condition is true.
    template <typename Pred>
    struct take_while_index
    {
        mutable Pred m_pred;

        take_while_index(Pred&& pred) : m_pred(std::forward<Pred>(pred)) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (std::size_t i{ 0 }; begin != end && m_pred(*begin, i); ++i, ++begin)
            {
                co_yield* begin;
            }
        };
    };

    template <typename Pred>
    take_while_index(Pred &&) -> take_while_index<Pred>;

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

            iterator_pack(It1 begin, It2 end) : m_begin(begin), m_end(end) {}
        };

        template <typename It1, typename It2>
        iterator_pack(It1, It2) -> iterator_pack<It1, It2>;
    } // namespace impl

    // Applies a specified function to the corresponding elements of two enumerable, producing an enumerable of the results.
    template <typename Selector, typename... Containers>
    struct zip
    {
        mutable Selector m_selector;
        std::tuple<impl::decay_container_t<Containers>...> m_cs;

        zip(Selector&& selector, Containers&&... cs)
            : m_selector(std::forward<Selector>(selector)),
              m_cs(impl::decay_container(std::forward<Containers>(cs))...) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<std::remove_cvref_t<decltype(m_selector(*std::begin(std::declval<Container&>()), *std::begin(std::declval<Containers&>())...))>>
        {
            auto its = std::apply(
                [&container](auto&&... cs) {
                    return std::make_tuple(
                        impl::iterator_pack{ std::begin(container), std::end(container) },
                        impl::iterator_pack{ std::begin(cs), std::end(cs) }...);
                },
                m_cs);
            for (;
                 std::apply([](auto&&... pack) { return impl::and_all((pack.m_begin != pack.m_end)...); }, its);
                 std::apply([](auto&&... pack) { return impl::expand_tuple((++pack.m_begin)...); }, its))
            {
                co_yield std::apply([this](auto&&... pack) { return m_selector((*pack.m_begin)...); }, its);
            }
        }
    };

    template <typename Selector, typename... Containers>
    zip(Selector&&, Containers&&...) -> zip<Selector, Containers...>;

    // Applies a specified function to the corresponding elements of two enumerable, producing an enumerable of the results.
    template <typename Selector, typename... Containers>
    struct zip_index
    {
        mutable Selector m_selector;
        std::tuple<impl::decay_container_t<Containers>...> m_cs;

        zip_index(Selector&& selector, Containers&&... cs)
            : m_selector(std::forward<Selector>(selector)),
              m_cs(impl::decay_container(std::forward<Containers>(cs))...) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<std::remove_cvref_t<decltype(m_selector(*std::begin(std::declval<Container&>()), *std::begin(std::declval<Containers&>())..., std::size_t{}))>>
        {
            auto its = std::apply(
                [&container](auto&&... cs) {
                    return std::make_tuple(
                        impl::iterator_pack{ std::begin(container), std::end(container) },
                        impl::iterator_pack{ std::begin(cs), std::end(cs) }...);
                },
                m_cs);
            for (std::size_t i{ 0 };
                 std::apply([](auto&&... pack) { return impl::and_all((pack.m_begin != pack.m_end)...); }, its);
                 ++i, std::apply([](auto&&... pack) { return impl::expand_tuple((++pack.m_begin)...); }, its))
            {
                co_yield std::apply([this, i](auto&&... pack) { return m_selector((*pack.m_begin)..., i); }, its);
            }
        }
    };

    template <typename Selector, typename... Containers>
    zip_index(Selector&&, Containers&&...) -> zip_index<Selector, Containers...>;
} // namespace linq

#endif // !LINQ_QUERY_HPP
