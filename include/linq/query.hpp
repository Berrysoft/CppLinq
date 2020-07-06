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
        template <typename It, typename Pred>
        class where_iterator_impl
        {
        private:
            It m_begin, m_end;
            std::decay_t<Pred> m_pred;

            void move_next_impl()
            {
                for (; m_begin != m_end; ++m_begin)
                {
                    if (m_pred(*m_begin)) break;
                }
            }

        public:
            using traits_type = std::iterator_traits<It>;

            where_iterator_impl(It begin, It end, Pred&& pred)
                : m_begin(begin), m_end(end), m_pred(std::forward<Pred>(pred)) { move_next_impl(); }

            typename traits_type::reference value() const noexcept { return *m_begin; }

            void move_next()
            {
                ++m_begin;
                move_next_impl();
            }

            bool is_valid() const { return m_begin != m_end; }
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

            void move_next_impl()
            {
                for (; m_begin != m_end; ++m_begin, ++m_index)
                {
                    if (m_pred(*m_begin, m_index)) break;
                }
            }

        public:
            using traits_type = std::iterator_traits<It>;

            where_index_iterator_impl(It begin, It end, Pred&& pred)
                : m_begin(begin), m_end(end), m_pred(std::forward<Pred>(pred)) { move_next_impl(); }

            typename traits_type::reference value() const noexcept { return *m_begin; }

            void move_next()
            {
                ++m_begin;
                ++m_index;
                move_next_impl();
            }

            bool is_valid() const { return m_begin != m_end; }
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
                : m_begin(begin), m_end(end), m_selector(std::forward<Selector>(selector)) { set_result(); }

            typename traits_type::reference value() const noexcept { return *m_result; }

            void move_next()
            {
                ++m_begin;
                set_result();
            }

            bool is_valid() const { return m_begin != m_end; }
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
                : m_begin(begin), m_end(end), m_selector(std::forward<Selector>(selector)) { set_result(); }

            typename traits_type::reference value() const noexcept { return *m_result; }

            void move_next()
            {
                ++m_begin;
                ++m_index;
                set_result();
            }

            bool is_valid() const { return m_begin != m_end; }
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
                    m_result1 = std::pointer_traits<result1_type>::pointer_to(*m_begin1);
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
                  m_cselector(std::forward<CSelector>(cselector)), m_rselector(std::forward<RSelector>(rselector))
            {
                set_container();
                set_result();
            }

            typename traits_type::reference value() const noexcept { return *m_result; }

            void move_next()
            {
                ++m_begin2;
                if (m_begin2 == m_end2)
                {
                    ++m_begin1;
                    set_container();
                }
                set_result();
            }

            bool is_valid() const { return m_begin1 != m_end1 || m_begin2 != m_end2; }
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
                    m_result1 = std::pointer_traits<result1_type>::pointer_to(*m_begin1);
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
                  m_cselector(std::forward<CSelector>(cselector)), m_rselector(std::forward<RSelector>(rselector))
            {
                set_container();
                set_result();
            }

            typename traits_type::reference value() const noexcept { return *m_result; }

            void move_next()
            {
                ++m_begin2;
                if (m_begin2 == m_end2)
                {
                    ++m_begin1;
                    set_container();
                }
                set_result();
            }

            bool is_valid() const { return m_begin1 != m_end1 || m_begin2 != m_end2; }
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

            void move_next() { ++m_begin; }

            bool is_valid() const { return m_begin != m_end; }
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

            void move_next() { ++m_begin; }

            bool is_valid() const { return m_begin != m_end; }
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

    namespace impl
    {
        template <typename It>
        class skip_while_index_iterator_impl
        {
        private:
            It m_begin, m_end;

        public:
            using traits_type = std::iterator_traits<It>;

            template <typename Pred>
            skip_while_index_iterator_impl(It begin, It end, Pred&& pred) : m_begin(begin), m_end(end)
            {
                for (std::size_t i = 0; m_begin != m_end && pred(*m_begin, i); ++m_begin, ++i)
                    ;
            }

            typename traits_type::reference value() const noexcept { return *m_begin; }

            void move_next() { ++m_begin; }

            bool is_valid() const { return m_begin != m_end; }
        };

        template <typename It>
        using skip_while_index_iterator = iterator_base<skip_while_index_iterator_impl<It>>;
    } // namespace impl

    // Bypasses elements as long as a specified condition is true and then returns the remaining elements.
    template <typename Pred>
    constexpr auto skip_while_index(Pred&& pred)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::skip_while_index_iterator<It>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<Pred>(pred) } };
        };
    }

    namespace impl
    {
        template <typename It>
        class take_iterator_impl
        {
        private:
            It m_begin, m_end;
            // skip doesn't need the number because it just skipped.
            std::size_t m_taken;

        public:
            using traits_type = std::iterator_traits<It>;

            take_iterator_impl(It begin, It end, std::size_t taken) : m_begin(begin), m_end(end), m_taken(taken) {}

            typename traits_type::reference value() { return *m_begin; }

            void move_next()
            {
                ++m_begin;
                --m_taken;
            }

            bool is_valid() const { return m_begin != m_end && m_taken; }
        };

        template <typename It>
        using take_iterator = iterator_base<take_iterator_impl<It>>;
    } // namespace impl

    // Returns a specified number of contiguous elements from the start.
    constexpr auto take(std::size_t taken)
    {
        return [=](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::take_iterator<It>{ impl::iterator_ctor, std::begin(container), std::end(container), taken } };
        };
    }

    namespace impl
    {
        template <typename It, typename Pred>
        class take_while_iterator_impl
        {
        private:
            It m_begin, m_end;
            std::decay_t<Pred> m_pred;

        public:
            using traits_type = std::iterator_traits<It>;

            take_while_iterator_impl(It begin, It end, Pred&& pred) : m_begin(begin), m_end(end), m_pred(std::forward<Pred>(pred)) {}

            typename traits_type::reference value() { return *m_begin; }

            void move_next() { ++m_begin; }

            bool is_valid() const { return m_begin != m_end && m_pred(*m_begin); }
        };

        template <typename It, typename Pred>
        using take_while_iterator = iterator_base<take_while_iterator_impl<It, Pred>>;
    } // namespace impl

    // Returns elements as long as a specified condition is true.
    template <typename Pred>
    constexpr auto take_while(Pred&& pred)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::take_while_iterator<It, Pred>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<Pred>(pred) } };
        };
    }

    namespace impl
    {
        template <typename It, typename Pred>
        class take_while_index_iterator_impl
        {
        private:
            It m_begin, m_end;
            std::decay_t<Pred> m_pred;
            std::size_t m_index{ 0 };

        public:
            using traits_type = std::iterator_traits<It>;

            take_while_index_iterator_impl(It begin, It end, Pred&& pred) : m_begin(begin), m_end(end), m_pred(std::forward<Pred>(pred)) {}

            typename traits_type::reference value() { return *m_begin; }

            void move_next()
            {
                ++m_begin;
                ++m_index;
            }

            bool is_valid() const { return m_begin != m_end && m_pred(*m_begin, m_index); }
        };

        template <typename It, typename Pred>
        using take_while_index_iterator = iterator_base<take_while_index_iterator_impl<It, Pred>>;
    } // namespace impl

    // Returns elements as long as a specified condition is true.
    template <typename Pred>
    constexpr auto take_while_index(Pred&& pred)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::take_while_index_iterator<It, Pred>{ impl::iterator_ctor, std::begin(container), std::end(container), std::forward<Pred>(pred) } };
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

        template <typename It>
        struct iterator_pack
        {
            It m_begin, m_end;
        };

        template <typename Selector, typename... Its>
        class zip_iterator_impl
        {
        private:
            std::decay_t<Selector> m_selector;
            std::tuple<iterator_pack<Its>...> m_its;

            using result_type = std::common_type_t<typename std::iterator_traits<Its>::value_type...>;
            std::optional<result_type> m_result{};

            void set_result()
            {
                if (is_valid())
                    m_result.emplace(std::apply([this](auto&&... pack) { return m_selector((*pack.m_begin)...); }, m_its));
            }

        public:
            using traits_type = iterator_impl_traits<result_type>;

            zip_iterator_impl(Selector&& selector, iterator_pack<Its>... its) : m_selector(std::forward<Selector>(selector)), m_its(std::make_tuple(its...)) { set_result(); }

            typename traits_type::reference value() const { return *m_result; }

            void move_next()
            {
                std::apply([](auto&&... pack) { return expand_tuple((++pack.m_begin)...); }, m_its);
                set_result();
            }

            bool is_valid() const
            {
                return std::apply([](auto&&... pack) { return and_all((pack.m_begin != pack.m_end)...); }, m_its);
            }
        };

        template <typename Selector, typename... Its>
        using zip_iterator = iterator_base<zip_iterator_impl<Selector, Its...>>;
    } // namespace impl

    // Applies a specified function to the corresponding elements of two enumerable, producing an enumerable of the results.
    template <typename Selector, typename... Containers>
    constexpr auto zip(Selector&& selector, Containers&&... cs)
    {
        return [&](auto&& container) {
            if constexpr (sizeof...(Containers) == 0)
            {
                return container;
            }
            else
            {
                using It = decltype(std::begin(container));
                return impl::iterable{ impl::zip_iterator<Selector, It, decltype(std::begin(cs))...>{ impl::iterator_ctor, std::forward<Selector>(selector), impl::iterator_pack<It>{ std::begin(container), std::end(container) }, impl::iterator_pack<decltype(std::begin(cs))>{ std::begin(cs), std::end(cs) }... } };
            }
        };
    }

    namespace impl
    {
        template <typename Selector, typename... Its>
        class zip_index_iterator_impl
        {
        private:
            std::decay_t<Selector> m_selector;
            std::tuple<iterator_pack<Its>...> m_its;
            std::size_t m_index{ 0 };

            using result_type = std::common_type_t<typename std::iterator_traits<Its>::value_type...>;
            std::optional<result_type> m_result{};

            void set_result()
            {
                if (is_valid())
                    m_result.emplace(std::apply([this](auto&&... pack) { return m_selector((*pack.m_begin)..., m_index); }, m_its));
            }

        public:
            using traits_type = iterator_impl_traits<result_type>;

            zip_index_iterator_impl(Selector&& selector, iterator_pack<Its>... its) : m_selector(std::move(selector)), m_its(std::make_tuple(its...)) { set_result(); }

            typename traits_type::reference value() const { return *m_result; }

            void move_next()
            {
                std::apply([](auto&&... pack) { return expand_tuple((++pack.m_begin)...); }, m_its);
                ++m_index;
                set_result();
            }

            bool is_valid() const
            {
                return std::apply([](auto&&... pack) { return and_all((pack.m_begin != pack.m_end)...); }, m_its);
            }
        };

        template <typename Selector, typename... Its>
        using zip_index_iterator = iterator_base<zip_index_iterator_impl<Selector, Its...>>;
    } // namespace impl

    // Applies a specified function to the corresponding elements of two enumerable, producing an enumerable of the results.
    template <typename Selector, typename... Containers>
    constexpr auto zip_index(Selector&& selector, Containers&&... cs)
    {
        return [&](auto&& container) {
            if constexpr (sizeof...(Containers) == 0)
            {
                return container;
            }
            else
            {
                using It = decltype(std::begin(container));
                return impl::iterable{ impl::zip_index_iterator<Selector, It, decltype(std::begin(cs))...>{ impl::iterator_ctor, std::forward<Selector>(selector), impl::iterator_pack<It>{ std::begin(container), std::end(container) }, impl::iterator_pack<decltype(std::begin(cs))>{ std::begin(cs), std::end(cs) }... } };
            }
        };
    }
} // namespace linq

#endif // !LINQ_QUERY_HPP
