/**CppLinq query.hpp
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
#ifndef LINQ_QUERY_HPP
#define LINQ_QUERY_HPP

#include <linq/core.hpp>
#include <tuple>

namespace linq
{
    namespace impl
    {
        template <typename It, typename Pred>
        class where_iterator_impl
        {
        private:
            It m_begin, m_end;
            std::decay_t<Pred> m_pred;

            bool move_next_impl()
            {
                for (; m_begin != m_end; ++m_begin)
                {
                    if (m_pred(*m_begin)) break;
                }
                return m_begin != m_end;
            }

        public:
            using traits_type = std::iterator_traits<It>;

            where_iterator_impl(It begin, It end, Pred&& pred)
                : m_begin(begin), m_end(end), m_pred(std::move(pred)) { move_next_impl(); }

            typename traits_type::reference value() const noexcept { return *m_begin; }

            bool move_next()
            {
                ++m_begin;
                return move_next_impl();
            }
        };

        template <typename It, typename Pred>
        using where_iterator = iterator_base<where_iterator_impl<It, Pred>>;
    } // namespace impl

    // Filters an enumerable based on a predicate.
    template <typename Pred>
    constexpr auto where(Pred&& pred)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::where_iterator<It, Pred>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<Pred>(pred) } };
        };
    }

    namespace impl
    {
        template <typename It, typename Pred>
        class where_index_iterator_impl
        {
        private:
            It m_begin, m_end;
            std::decay_t<Pred> m_pred;
            std::size_t m_index{ 0 };

            bool move_next_impl()
            {
                for (; m_begin != m_end; ++m_begin, ++m_index)
                {
                    if (m_pred(*m_begin, m_index)) break;
                }
                return m_begin != m_end;
            }

        public:
            using traits_type = std::iterator_traits<It>;

            where_index_iterator_impl(It begin, It end, Pred&& pred)
                : m_begin(begin), m_end(end), m_pred(std::move(pred)) { move_next_impl(); }

            typename traits_type::reference value() const noexcept { return *m_begin; }

            bool move_next()
            {
                ++m_begin;
                ++m_index;
                return move_next_impl();
            }
        };

        template <typename It, typename Pred>
        using where_index_iterator = iterator_base<where_index_iterator_impl<It, Pred>>;
    } // namespace impl

    // Filters an enumerable based on a predicate.
    template <typename Pred>
    constexpr auto where_index(Pred&& pred)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::where_index_iterator<It, Pred>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<Pred>(pred) } };
        };
    }

    namespace impl
    {
        template <typename It, typename Selector>
        class select_iterator_impl
        {
        private:
            It m_begin, m_end;
            std::decay_t<Selector> m_selector;

            using result_type = std::remove_cv_t<decltype(std::declval<Selector>()(std::declval<typename std::iterator_traits<It>::reference>()))>;

            std::optional<result_type> m_result{};

            void set_result()
            {
                if (m_begin != m_end)
                    m_result = m_selector(*m_begin);
            }

        public:
            using traits_type = iterator_impl_traits<result_type>;

            select_iterator_impl(It begin, It end, Selector&& selector)
                : m_begin(begin), m_end(end), m_selector(std::move(selector)) { set_result(); }

            typename traits_type::reference value() const noexcept { return *m_result; }

            bool move_next()
            {
                if (m_begin != m_end)
                {
                    ++m_begin;
                    set_result();
                }
                return m_begin != m_end;
            }
        };

        template <typename It, typename Selector>
        using select_iterator = iterator_base<select_iterator_impl<It, Selector>>;
    } // namespace impl

    // Projects each element into a new form.
    template <typename Selector>
    constexpr auto select(Selector&& selector)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::select_iterator<It, Selector>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<Selector>(selector) } };
        };
    }

    namespace impl
    {
        template <typename It, typename Selector>
        class select_index_iterator_impl
        {
        private:
            It m_begin, m_end;
            std::decay_t<Selector> m_selector;
            std::size_t m_index{ 0 };

            using result_type = std::remove_cv_t<decltype(std::declval<Selector>()(std::declval<typename std::iterator_traits<It>::reference>(), m_index))>;

            std::optional<result_type> m_result{};

            void set_result()
            {
                if (m_begin != m_end)
                    m_result = m_selector(*m_begin, m_index);
            }

        public:
            using traits_type = iterator_impl_traits<result_type>;

            select_index_iterator_impl(It begin, It end, Selector&& selector)
                : m_begin(begin), m_end(end), m_selector(std::move(selector)) { set_result(); }

            typename traits_type::reference value() const noexcept { return *m_result; }

            bool move_next()
            {
                if (m_begin != m_end)
                {
                    ++m_begin;
                    ++m_index;
                    set_result();
                }
                return m_begin != m_end;
            }
        };

        template <typename It, typename Selector>
        using select_index_iterator = iterator_base<select_index_iterator_impl<It, Selector>>;
    } // namespace impl

    // Projects each element into a new form.
    template <typename Selector>
    constexpr auto select_index(Selector&& selector)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::select_index_iterator<It, Selector>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<Selector>(selector) } };
        };
    }

    namespace impl
    {
        template <typename It1, typename It2, typename CSelector, typename RSelector>
        class select_many_iterator_impl
        {
        private:
            It1 m_begin1, m_end1;
            It2 m_begin2, m_end2;
            std::decay_t<CSelector> m_cselector;
            std::decay_t<RSelector> m_rselector;

            using container_type = std::remove_reference_t<decltype(m_cselector(*m_begin1))>;
            std::optional<container_type> m_container{};

            using result1_type = typename std::iterator_traits<It1>::pointer;
            result1_type m_result1{};
            using result_type = decltype(m_rselector(*m_result1, *m_begin2));
            std::optional<result_type> m_result{};

            void set_container()
            {
                if (m_begin1 != m_end1)
                {
                    m_result1 = &*m_begin1;
                    m_container.emplace(m_cselector(*m_result1));
                    m_begin2 = std::begin(*m_container);
                    m_end2 = std::end(*m_container);
                }
                else
                {
                    m_container = std::nullopt;
                }
            }

            void set_result()
            {
                if (m_begin2 != m_end2)
                    m_result.emplace(m_rselector(*m_result1, *m_begin2));
            }

        public:
            using traits_type = iterator_impl_traits<result_type>;

            select_many_iterator_impl(It1 begin, It1 end, CSelector&& cselector, RSelector&& rselector)
                : m_begin1(begin), m_end1(end),
                  m_cselector(std::move(cselector)), m_rselector(std::move(rselector))
            {
                set_container();
                set_result();
            }

            typename traits_type::reference value() const noexcept { return *m_result; }

            bool move_next()
            {
                ++m_begin2;
                if (m_begin2 == m_end2)
                {
                    ++m_begin1;
                    set_container();
                }
                set_result();
                return m_begin1 != m_end1 || m_begin2 != m_end2;
            }
        };

        template <typename It1, typename It2, typename CSelector, typename RSelector>
        using select_many_iterator = iterator_base<select_many_iterator_impl<It1, It2, CSelector, RSelector>>;
    } // namespace impl

    // Projects each element, flattens the resulting sequences into one sequence, and invokes a result selector function on each element therein.
    template <typename CSelector, typename RSelector>
    constexpr auto select_many(CSelector&& cselector, RSelector&& rselector)
    {
        return [&](auto&& container) {
            using It1 = decltype(std::begin(container));
            using It2 = decltype(std::begin(cselector(*std::begin(container))));
            return impl::iterable{ impl::select_many_iterator<It1, It2, CSelector, RSelector>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<CSelector>(cselector), std::forward<RSelector>(rselector) } };
        };
    }

    namespace impl
    {
        template <typename It1, typename It2, typename CSelector, typename RSelector>
        class select_many_index_iterator_impl
        {
        private:
            It1 m_begin1, m_end1;
            It2 m_begin2, m_end2;
            std::decay_t<CSelector> m_cselector;
            std::decay_t<RSelector> m_rselector;
            std::size_t m_index{ 0 };

            using container_type = std::remove_reference_t<decltype(m_cselector(*m_begin1, m_index))>;
            std::optional<container_type> m_container{};

            using result1_type = typename std::iterator_traits<It1>::pointer;
            result1_type m_result1{};
            using result_type = decltype(m_rselector(*m_result1, *m_begin2));
            std::optional<result_type> m_result{};

            void set_container()
            {
                if (m_begin1 != m_end1)
                {
                    m_result1 = &*m_begin1;
                    m_container.emplace(m_cselector(*m_result1, m_index));
                    m_index++;
                    m_begin2 = std::begin(*m_container);
                    m_end2 = std::end(*m_container);
                }
                else
                {
                    m_container = std::nullopt;
                }
            }

            void set_result()
            {
                if (m_begin2 != m_end2)
                    m_result.emplace(m_rselector(*m_result1, *m_begin2));
            }

        public:
            using traits_type = iterator_impl_traits<result_type>;

            select_many_index_iterator_impl(It1 begin, It1 end, CSelector&& cselector, RSelector&& rselector)
                : m_begin1(begin), m_end1(end),
                  m_cselector(std::move(cselector)), m_rselector(std::move(rselector))
            {
                set_container();
                set_result();
            }

            typename traits_type::reference value() const noexcept { return *m_result; }

            bool move_next()
            {
                ++m_begin2;
                if (m_begin2 == m_end2)
                {
                    ++m_begin1;
                    set_container();
                }
                set_result();
                return m_begin1 != m_end1 || m_begin2 != m_end2;
            }
        };

        template <typename It1, typename It2, typename CSelector, typename RSelector>
        using select_many_index_iterator = iterator_base<select_many_index_iterator_impl<It1, It2, CSelector, RSelector>>;
    } // namespace impl

    // Projects each element, flattens the resulting sequences into one sequence, and invokes a result selector function on each element therein.
    template <typename CSelector, typename RSelector>
    constexpr auto select_many_index(CSelector&& cselector, RSelector&& rselector)
    {
        return [&](auto&& container) {
            using It1 = decltype(std::begin(container));
            using It2 = decltype(std::begin(cselector(*std::begin(container), std::size_t{})));
            return impl::iterable{ impl::select_many_index_iterator<It1, It2, CSelector, RSelector>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<CSelector>(cselector), std::forward<RSelector>(rselector) } };
        };
    }

    namespace impl
    {
        template <typename It>
        class skip_iterator_impl
        {
        private:
            It m_begin, m_end;

        public:
            using traits_type = std::iterator_traits<It>;

            skip_iterator_impl(It begin, It end, std::size_t skipn) : m_begin(begin), m_end(end)
            {
                for (; m_begin != m_end && skipn--; ++m_begin)
                    ;
            }

            typename traits_type::reference value() const noexcept { return *m_begin; }

            bool move_next()
            {
                if (m_begin != m_end)
                    ++m_begin;
                return m_begin != m_end;
            }
        };

        template <typename It>
        using skip_iterator = iterator_base<skip_iterator_impl<It>>;
    } // namespace impl

    // Bypasses a specified number of elements and then returns the remaining elements.
    constexpr auto skip(std::size_t skipn)
    {
        return [=](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::skip_iterator<It>{ impl::iterator_ctor, std::begin(container), std::end(container), skipn } };
        };
    }

    namespace impl
    {
        template <typename It>
        class skip_while_iterator_impl
        {
        private:
            It m_begin, m_end;

        public:
            using traits_type = std::iterator_traits<It>;

            template <typename Pred>
            skip_while_iterator_impl(It begin, It end, Pred&& pred) : m_begin(begin), m_end(end)
            {
                for (; m_begin != m_end && pred(*m_begin); ++m_begin)
                    ;
            }

            typename traits_type::reference value() const noexcept { return *m_begin; }

            bool move_next()
            {
                if (m_begin != m_end)
                    ++m_begin;
                return m_begin != m_end;
            }
        };

        template <typename It>
        using skip_while_iterator = iterator_base<skip_while_iterator_impl<It>>;
    } // namespace impl

    // Bypasses elements as long as a specified condition is true and then returns the remaining elements.
    template <typename Pred>
    constexpr auto skip_while(Pred&& pred)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::skip_while_iterator<It>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<Pred>(pred) } };
        };
    }

    //namespace impl
    //{
    //    template <typename Eter>
    //    class skip_while_index_enumerator
    //    {
    //    private:
    //        Eter m_eter;

    //    public:
    //        template <typename Pred>
    //        constexpr skip_while_index_enumerator(Eter&& eter, Pred&& pred) : m_eter(std::forward<Eter>(eter))
    //        {
    //            for (std::size_t i{ 0 }; m_eter && pred(*m_eter, i); ++m_eter, ++i)
    //                ;
    //        }

    //        constexpr operator bool() const { return m_eter; }
    //        constexpr skip_while_index_enumerator& operator++()
    //        {
    //            ++m_eter;
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*() { return *m_eter; }
    //    };
    //} // namespace impl

    //// Bypasses elements as long as a specified condition is true and then returns the remaining elements.
    //template <typename Pred>
    //constexpr auto skip_while_index(Pred&& pred)
    //{
    //    return [&](auto e) {
    //        using Eter = decltype(e.enumerator());
    //        return enumerable(impl::skip_while_index_enumerator<Eter>(e.enumerator(), std::forward<Pred>(pred)));
    //    };
    //}

    //namespace impl
    //{
    //    template <typename Eter>
    //    class take_enumerator
    //    {
    //    private:
    //        Eter m_eter;
    //        // skip doesn't need the number because it just skipped.
    //        std::size_t m_taken;

    //    public:
    //        constexpr take_enumerator(Eter&& eter, std::size_t taken) : m_eter(std::forward<Eter>(eter)), m_taken(taken) {}

    //        constexpr operator bool() const { return m_eter && m_taken; }
    //        constexpr take_enumerator& operator++()
    //        {
    //            ++m_eter;
    //            --m_taken;
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*() { return *m_eter; }
    //    };
    //} // namespace impl

    //// Returns a specified number of contiguous elements from the start.
    //constexpr auto take(std::size_t taken)
    //{
    //    return [=](auto e) {
    //        using Eter = decltype(e.enumerator());
    //        return enumerable(impl::take_enumerator<Eter>(e.enumerator(), taken));
    //    };
    //}

    //namespace impl
    //{
    //    template <typename Eter, typename Pred>
    //    class take_while_enumerator
    //    {
    //    private:
    //        Eter m_eter;
    //        Pred m_pred;

    //    public:
    //        constexpr take_while_enumerator(Eter&& eter, Pred&& pred) : m_eter(std::forward<Eter>(eter)), m_pred(pred) {}

    //        constexpr operator bool() { return m_eter && m_pred(*m_eter); }
    //        constexpr take_while_enumerator& operator++()
    //        {
    //            ++m_eter;
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*() { return *m_eter; }
    //    };
    //} // namespace impl

    //// Returns elements as long as a specified condition is true.
    //template <typename Pred>
    //constexpr auto take_while(Pred&& pred)
    //{
    //    return [&](auto e) {
    //        using Eter = decltype(e.enumerator());
    //        return enumerable(impl::take_while_enumerator<Eter, Pred>(e.enumerator(), std::forward<Pred>(pred)));
    //    };
    //}

    //namespace impl
    //{
    //    template <typename Eter, typename Pred>
    //    class take_while_index_enumerator
    //    {
    //    private:
    //        Eter m_eter;
    //        Pred m_pred;
    //        std::size_t m_index;

    //    public:
    //        constexpr take_while_index_enumerator(Eter&& eter, Pred&& pred) : m_eter(std::forward<Eter>(eter)), m_pred(pred), m_index(0) {}

    //        constexpr operator bool() { return m_eter && m_pred(*m_eter, m_index); }
    //        constexpr take_while_index_enumerator& operator++()
    //        {
    //            ++m_eter;
    //            ++m_index;
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*() { return *m_eter; }
    //    };
    //} // namespace impl

    //// Returns elements as long as a specified condition is true.
    //template <typename Pred>
    //constexpr auto take_while_index(Pred&& pred)
    //{
    //    return [&](auto e) {
    //        using Eter = decltype(e.enumerator());
    //        return enumerable(impl::take_while_index_enumerator<Eter, Pred>(e.enumerator(), std::forward<Pred>(pred)));
    //    };
    //}

    //namespace impl
    //{
    //    template <typename E1, typename... Es>
    //    constexpr bool and_all(E1&& e1, Es&&... es)
    //    {
    //        if constexpr (sizeof...(Es) == 0)
    //        {
    //            return e1;
    //        }
    //        else
    //        {
    //            return e1 && and_all<Es...>(std::forward<Es>(es)...);
    //        }
    //    }

    //    template <typename... T>
    //    constexpr void expand_tuple(T&&...)
    //    {
    //    }

    //    template <typename Selector, typename... Eters>
    //    class zip_enumerator
    //    {
    //    private:
    //        Selector m_selector;
    //        std::tuple<Eters...> m_eters;

    //    public:
    //        constexpr zip_enumerator(Selector&& selector, Eters&&... eters) : m_selector(selector), m_eters(std::make_tuple(std::forward<Eters>(eters)...)) {}

    //        constexpr operator bool() const
    //        {
    //            return std::apply([](auto&&... e) { return and_all(e...); }, m_eters);
    //        }
    //        constexpr zip_enumerator& operator++()
    //        {
    //            std::apply([](auto&&... e) { return expand_tuple(++e...); }, m_eters);
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*()
    //        {
    //            return std::apply([this](auto&&... e) { return m_selector(*e...); }, m_eters);
    //        }
    //    };
    //} // namespace impl

    //// Applies a specified function to the corresponding elements of two enumerable, producing an enumerable of the results.
    //template <typename Selector, typename... Es>
    //constexpr auto zip(Selector&& selector, Es&&... es)
    //{
    //    return [&](auto e) {
    //        if constexpr (sizeof...(Es) == 0)
    //        {
    //            return e;
    //        }
    //        else
    //        {
    //            using Eter = decltype(e.enumerator());
    //            return enumerable(impl::zip_enumerator<Selector, Eter, decltype(get_enumerator(std::forward<Es>(es)))...>(std::forward<Selector>(selector), e.enumerator(), get_enumerator(std::forward<Es>(es))...));
    //        }
    //    };
    //}

    //namespace impl
    //{
    //    template <typename Selector, typename... Eters>
    //    class zip_index_enumerator
    //    {
    //    private:
    //        Selector m_selector;
    //        std::tuple<Eters...> m_eters;
    //        std::size_t m_index;

    //    public:
    //        constexpr zip_index_enumerator(Selector&& selector, Eters&&... eters) : m_selector(selector), m_eters(std::make_tuple(std::forward<Eters>(eters)...)), m_index(0) {}

    //        constexpr operator bool() const
    //        {
    //            return std::apply([](auto&&... e) { return and_all(e...); }, m_eters);
    //        }
    //        constexpr zip_index_enumerator& operator++()
    //        {
    //            std::apply([](auto&&... e) { return expand_tuple(++e...); }, m_eters);
    //            ++m_index;
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*()
    //        {
    //            return std::apply([this](auto&&... e) { return m_selector(*e..., m_index); }, m_eters);
    //        }
    //    };
    //} // namespace impl

    //// Applies a specified function to the corresponding elements of two enumerable, producing an enumerable of the results.
    //template <typename Selector, typename... Es>
    //constexpr auto zip_index(Selector&& selector, Es&&... es)
    //{
    //    return [&](auto e) {
    //        if constexpr (sizeof...(Es) == 0)
    //        {
    //            return e;
    //        }
    //        else
    //        {
    //            using Eter = decltype(e.enumerator());
    //            return enumerable(impl::zip_index_enumerator<Selector, Eter, decltype(get_enumerator(std::forward<Es>(es)))...>(std::forward<Selector>(selector), e.enumerator(), get_enumerator(std::forward<Es>(es))...));
    //        }
    //    };
    //}

    //namespace impl
    //{
    //    template <typename Eter, typename TTo>
    //    class cast_enumerator
    //    {
    //    private:
    //        Eter m_eter;

    //    public:
    //        cast_enumerator(Eter&& eter) : m_eter(std::forward<Eter>(eter)) {}

    //        constexpr operator bool() const { return m_eter; }
    //        constexpr cast_enumerator& operator++()
    //        {
    //            ++m_eter;
    //            return *this;
    //        }
    //        constexpr TTo operator*() { return (TTo)*m_eter; }
    //    };
    //} // namespace impl

    //// Applies C-style cast to the elements.
    //template <typename TTo>
    //constexpr auto cast()
    //{
    //    return [](auto e) {
    //        using Eter = decltype(e.enumerator());
    //        return enumerable(impl::cast_enumerator<Eter, TTo>(e.enumerator()));
    //    };
    //}
} // namespace linq

#endif // !LINQ_QUERY_HPP
