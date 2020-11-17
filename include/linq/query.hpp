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
    namespace impl
    {
        template <impl::container Container, typename Pred>
        auto where(Container container, Pred pred)
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            for (auto&& item : container)
            {
                if (pred(item)) co_yield item;
            }
        }
    } // namespace impl

    // Filters an enumerable based on a predicate.
    template <typename Pred>
    auto where(Pred&& pred)
    {
        return [=]<impl::container Container>(Container&& container) {
            return impl::where<Container, Pred>(std::forward<Container>(container), impl::move_const(pred));
        };
    }

    namespace impl
    {
        template <impl::container Container, typename Pred>
        auto where_index(Container container, Pred pred)
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            std::size_t index{ 0 };
            for (auto&& item : container)
            {
                if (pred(item, index)) co_yield item;
                index++;
            }
        }
    } // namespace impl

    // Filters an enumerable based on a predicate.
    template <typename Pred>
    auto where_index(Pred&& pred)
    {
        return [=]<impl::container Container>(Container&& container) {
            return impl::where_index<Container, Pred>(std::forward<Container>(container), impl::move_const(pred));
        };
    }

    namespace impl
    {
        template <impl::container Container, typename Selector>
        auto select(Container container, Selector selector)
            -> generator<std::remove_cvref_t<decltype(selector(*std::begin(container)))>>
        {
            for (auto&& item : container)
            {
                co_yield selector(item);
            }
        }
    } // namespace impl

    // Projects each element into a new form.
    template <typename Selector>
    auto select(Selector&& selector)
    {
        return [=]<impl::container Container>(Container&& container) {
            return impl::select<Container, Selector>(std::forward<Container>(container), impl::move_const(selector));
        };
    }

    namespace impl
    {
        template <impl::container Container, typename Selector>
        auto select_index(Container container, Selector selector)
            -> generator<std::remove_cvref_t<decltype(selector(*std::begin(container), std::size_t{}))>>
        {
            std::size_t index{ 0 };
            for (auto&& item : container)
            {
                co_yield selector(item, index);
                index++;
            }
        }
    } // namespace impl

    // Projects each element into a new form.
    template <typename Selector>
    auto select_index(Selector&& selector)
    {
        return [=]<impl::container Container>(Container&& container) {
            return impl::select_index<Container, Selector>(std::forward<Container>(container), impl::move_const(selector));
        };
    }

    namespace impl
    {
        template <impl::container Container, typename CSelector, typename RSelector>
        auto select_many(Container container, CSelector cselector, RSelector rselector)
            -> generator<std::remove_cvref_t<decltype(rselector(*std::begin(container), *std::begin(cselector(*std::begin(container)))))>>
        {
            for (auto&& c : container)
            {
                for (auto&& item : cselector(c))
                {
                    co_yield rselector(c, item);
                }
            }
        }
    } // namespace impl

    // Projects each element, flattens the resulting sequences into one sequence, and invokes a result selector function on each element therein.
    template <typename CSelector, typename RSelector>
    auto select_many(CSelector&& cselector, RSelector&& rselector)
    {
        return [=]<impl::container Container>(Container&& container) {
            return impl::select_many<Container, CSelector, RSelector>(std::forward<Container>(container), impl::move_const(cselector), impl::move_const(rselector));
        };
    }

    namespace impl
    {
        template <impl::container Container, typename CSelector, typename RSelector>
        auto select_many_index(Container container, CSelector cselector, RSelector rselector)
            -> generator<std::remove_cvref_t<decltype(rselector(*std::begin(container), *std::begin(cselector(*std::begin(container), std::size_t{}))))>>
        {
            std::size_t index{ 0 };
            for (auto&& c : container)
            {
                for (auto&& item : cselector(c, index))
                {
                    co_yield rselector(c, item);
                }
                index++;
            }
        }
    } // namespace impl

    // Projects each element, flattens the resulting sequences into one sequence, and invokes a result selector function on each element therein.
    template <typename CSelector, typename RSelector>
    auto select_many_index(CSelector&& cselector, RSelector&& rselector)
    {
        return [=]<impl::container Container>(Container&& container) {
            return impl::select_many_index<Container, CSelector, RSelector>(std::forward<Container>(container), impl::move_const(cselector), impl::move_const(rselector));
        };
    }

    namespace impl
    {
        template <impl::container Container>
        auto skip(Container container, std::size_t n)
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && n--; ++begin)
                ;
            for (; begin != end; ++begin)
            {
                co_yield* begin;
            }
        }
    } // namespace impl

    // Bypasses a specified number of elements and then returns the remaining elements.
    inline auto skip(std::size_t skipn)
    {
        return [=]<impl::container Container>(Container&& container) {
            return impl::skip<Container>(std::forward<Container>(container), skipn);
        };
    }

    namespace impl
    {
        template <impl::container Container, typename Pred>
        auto skip_while(Container container, Pred pred)
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && pred(*begin); ++begin)
                ;
            for (; begin != end; ++begin)
            {
                co_yield* begin;
            }
        }
    } // namespace impl

    // Bypasses elements as long as a specified condition is true and then returns the remaining elements.
    template <typename Pred>
    auto skip_while(Pred&& pred)
    {
        return [=]<impl::container Container>(Container&& container) {
            return impl::skip_while<Container, Pred>(std::forward<Container>(container), impl::move_const(pred));
        };
    }

    namespace impl
    {
        template <impl::container Container, typename Pred>
        auto skip_while_index(Container container, Pred pred)
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (std::size_t i{ 0 }; begin != end && pred(*begin, i); ++i, ++begin)
                ;
            for (; begin != end; ++begin)
            {
                co_yield* begin;
            }
        }
    } // namespace impl

    // Bypasses elements as long as a specified condition is true and then returns the remaining elements.
    template <typename Pred>
    auto skip_while_index(Pred&& pred)
    {
        return [=]<impl::container Container>(Container&& container) {
            return impl::skip_while_index<Container, Pred>(std::forward<Container>(container), impl::move_const(pred));
        };
    }

    namespace impl
    {
        template <impl::container Container>
        auto take(Container container, std::size_t n)
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && n--; ++begin)
            {
                co_yield* begin;
            }
        }
    } // namespace impl

    // Returns a specified number of contiguous elements from the start.
    inline auto take(std::size_t taken)
    {
        return [=]<impl::container Container>(Container&& container) {
            return impl::take<Container>(std::forward<Container>(container), taken);
        };
    }

    namespace impl
    {
        template <impl::container Container, typename Pred>
        auto take_while(Container container, Pred pred)
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (; begin != end && pred(*begin); ++begin)
            {
                co_yield* begin;
            }
        }
    } // namespace impl

    // Returns elements as long as a specified condition is true.
    template <typename Pred>
    auto take_while(Pred&& pred)
    {
        return [=]<impl::container Container>(Container&& container) {
            return impl::take_while<Container, Pred>(std::forward<Container>(container), impl::move_const(pred));
        };
    }

    namespace impl
    {
        template <impl::container Container, typename Pred>
        auto take_while_index(Container container, Pred pred)
            -> generator<typename impl::container_traits<Container>::value_type>
        {
            auto begin = std::begin(container);
            auto end = std::end(container);
            for (std::size_t i{ 0 }; begin != end && pred(*begin, i); ++i, ++begin)
            {
                co_yield* begin;
            }
        }
    } // namespace impl

    // Returns elements as long as a specified condition is true.
    template <typename Pred>
    auto take_while_index(Pred&& pred)
    {
        return [=]<impl::container Container>(Container&& container) {
            return impl::take_while_index<Container, Pred>(std::forward<Container>(container), impl::move_const(pred));
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

            iterator_pack(It1 begin, It2 end) : m_begin(begin), m_end(end) {}
        };

        template <typename It1, typename It2>
        iterator_pack(It1, It2) -> iterator_pack<It1, It2>;
    } // namespace impl

    namespace impl
    {
        template <typename Selector, impl::container... Containers>
        auto zip(Selector selector, Containers... cs)
            -> generator<std::remove_cvref_t<decltype(selector(*std::begin(cs)...))>>
        {
            auto its = std::make_tuple(impl::iterator_pack{ std::begin(cs), std::end(cs) }...);
            for (;
                 std::apply([](auto&&... pack) { return impl::and_all((pack.m_begin != pack.m_end)...); }, its);
                 std::apply([](auto&&... pack) { return impl::expand_tuple((++pack.m_begin)...); }, its))
            {
                co_yield std::apply([&selector](auto&&... pack) { return selector((*pack.m_begin)...); }, its);
            }
        }
    } // namespace impl

    // Applies a specified function to the corresponding elements of two enumerable, producing an enumerable of the results.
    template <typename Selector, impl::container... Containers>
    auto zip(Selector&& selector, Containers&&... cs)
    {
        return [=, ... cs = impl::decay_container(std::forward<Containers>(cs))]<impl::container Container>(Container&& container) {
            return impl::zip<Selector, impl::decay_container_t<Container>, impl::decay_container_t<Containers>...>(impl::move_const(selector), std::forward<Container>(container), impl::move_const(cs)...);
        };
    }

    namespace impl
    {
        template <typename Selector, impl::container... Containers>
        auto zip_index(Selector selector, Containers... cs)
            -> generator<std::remove_cvref_t<decltype(selector(*std::begin(cs)..., std::size_t{}))>>
        {
            auto its = std::make_tuple(impl::iterator_pack{ std::begin(cs), std::end(cs) }...);
            for (std::size_t i{ 0 };
                 std::apply([](auto&&... pack) { return impl::and_all((pack.m_begin != pack.m_end)...); }, its);
                 ++i, std::apply([](auto&&... pack) { return impl::expand_tuple((++pack.m_begin)...); }, its))
            {
                co_yield std::apply([i, &selector](auto&&... pack) { return selector((*pack.m_begin)..., i); }, its);
            }
        }
    } // namespace impl

    // Applies a specified function to the corresponding elements of two enumerable, producing an enumerable of the results.
    template <typename Selector, impl::container... Containers>
    auto zip_index(Selector&& selector, Containers&&... cs)
    {
        return [=, ... cs = impl::decay_container(std::forward<Containers>(cs))]<impl::container Container>(Container&& container) {
            return impl::zip_index<Selector, impl::decay_container_t<Container>, impl::decay_container_t<Containers>...>(impl::move_const(selector), std::forward<Container>(container), impl::move_const(cs)...);
        };
    }
} // namespace linq

#endif // !LINQ_QUERY_HPP
