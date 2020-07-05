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
        class where_iterator : public iterator_base<where_iterator<It, Pred>>
        {
        private:
            It m_begin{}, m_end{};
            std::shared_ptr<Pred> m_pred{};

            void move_next()
            {
                while (m_begin != m_end && !(*m_pred)(*m_begin)) ++m_begin;
                if (m_begin == m_end) this->m_valid = false;
            }

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = typename std::iterator_traits<It>::value_type;
            using difference_type = typename std::iterator_traits<It>::difference_type;
            using pointer = typename std::iterator_traits<It>::pointer;
            using reference = typename std::iterator_traits<It>::reference;

            where_iterator() {}
            where_iterator(It begin, It end, Pred pred)
                : iterator_base<where_iterator>(true), m_begin(begin), m_end(end),
                  m_pred(std::make_shared<Pred>(std::move(pred))) { move_next(); }
            where_iterator(const where_iterator&) = default;
            where_iterator& operator=(const where_iterator&) = default;

            reference operator*() const noexcept { return *m_begin; }

            constexpr where_iterator& operator++()
            {
                ++m_begin;
                move_next();
                return *this;
            }
        };
    } // namespace impl

    // Filters an enumerable based on a predicate.
    template <typename Pred>
    constexpr auto where(Pred&& pred)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable<impl::where_iterator<It, Pred>>{ impl::where_iterator<It, Pred>{ std::begin(container), std::end(container), std::forward<Pred>(pred) } };
        };
    }

    namespace impl
    {
        template <typename It, typename Pred>
        class where_index_iterator : public iterator_base<where_index_iterator<It, Pred>>
        {
        private:
            It m_begin{}, m_end{};
            std::shared_ptr<Pred> m_pred{};
            std::size_t m_index{ 0 };

            constexpr void move_next()
            {
                for (; m_begin != m_end; ++m_begin, ++m_index)
                {
                    if ((*m_pred)(*m_begin, m_index)) break;
                }
                if (m_begin == m_end) this->m_valid = false;
            }

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = typename std::iterator_traits<It>::value_type;
            using difference_type = typename std::iterator_traits<It>::difference_type;
            using pointer = typename std::iterator_traits<It>::pointer;
            using reference = typename std::iterator_traits<It>::reference;

            where_index_iterator() {}
            where_index_iterator(It begin, It end, Pred pred)
                : iterator_base<where_index_iterator>(true), m_begin(begin), m_end(end),
                  m_pred(std::make_shared<Pred>(std::move(pred))) { move_next(); }
            where_index_iterator(const where_index_iterator&) = default;
            where_index_iterator& operator=(const where_index_iterator&) = default;

            reference operator*() const noexcept { return *m_begin; }

            constexpr where_index_iterator& operator++()
            {
                ++m_begin;
                ++m_index;
                move_next();
                return *this;
            }
        };
    } // namespace impl

    // Filters an enumerable based on a predicate.
    template <typename Pred>
    constexpr auto where_index(Pred&& pred)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable<impl::where_index_iterator<It, Pred>>{ impl::where_index_iterator<It, Pred>{ std::begin(container), std::end(container), std::forward<Pred>(pred) } };
        };
    }

    namespace impl
    {
        template <typename It, typename Selector>
        class select_iterator : public iterator_base<select_iterator<It, Selector>>
        {
        private:
            It m_begin{}, m_end{};
            std::shared_ptr<Selector> m_selector{};

            using result_type = std::remove_cv_t<decltype(std::declval<Selector>()(std::declval<typename std::iterator_traits<It>::reference>()))>;

            std::optional<result_type> m_result{};

            void set_result()
            {
                if (m_begin == m_end)
                {
                    m_result = std::nullopt;
                    this->m_valid = false;
                }
                else
                    m_result = (*m_selector)(*m_begin);
            }

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = result_type;
            using difference_type = typename std::iterator_traits<It>::difference_type;
            using pointer = const result_type*;
            using reference = const result_type&;

            select_iterator() {}
            select_iterator(It begin, It end, Selector selector)
                : iterator_base<select_iterator>(true), m_begin(begin), m_end(end),
                  m_selector(std::make_shared<Selector>(std::move(selector))) { set_result(); }
            select_iterator(const select_iterator&) = default;
            select_iterator& operator=(const select_iterator&) = default;

            reference operator*() const noexcept { return *m_result; }

            constexpr select_iterator& operator++()
            {
                ++m_begin;
                set_result();
                return *this;
            }
        };
    } // namespace impl

    // Projects each element into a new form.
    template <typename Selector>
    constexpr auto select(Selector&& selector)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable<impl::select_iterator<It, Selector>>{ impl::select_iterator<It, Selector>{ std::begin(container), std::end(container), std::forward<Selector>(selector) } };
        };
    }

    namespace impl
    {
        template <typename It, typename Selector>
        class select_index_iterator : public iterator_base<select_index_iterator<It, Selector>>
        {
        private:
            It m_begin{}, m_end{};
            std::shared_ptr<Selector> m_selector{};
            std::size_t m_index{ 0 };

            using result_type = std::remove_cv_t<decltype(std::declval<Selector>()(std::declval<typename std::iterator_traits<It>::reference>(), m_index))>;

            std::optional<result_type> m_result{};

            void set_result()
            {
                if (m_begin == m_end)
                {
                    m_result = std::nullopt;
                    this->m_valid = false;
                }
                else
                    m_result = (*m_selector)(*m_begin, m_index);
            }

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = result_type;
            using difference_type = typename std::iterator_traits<It>::difference_type;
            using pointer = const result_type*;
            using reference = const result_type&;

            select_index_iterator() {}
            select_index_iterator(It begin, It end, Selector selector)
                : iterator_base<select_index_iterator>(true), m_begin(begin), m_end(end),
                  m_selector(std::make_shared<Selector>(std::move(selector))) { set_result(); }
            select_index_iterator(const select_index_iterator&) = default;
            select_index_iterator& operator=(const select_index_iterator&) = default;

            reference operator*() const noexcept { return *m_result; }

            constexpr select_index_iterator& operator++()
            {
                ++m_begin;
                ++m_index;
                set_result();
                return *this;
            }
        };
    } // namespace impl

    // Projects each element into a new form.
    template <typename Selector>
    constexpr auto select_index(Selector&& selector)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable<impl::select_index_iterator<It, Selector>>{ impl::select_index_iterator<It, Selector>{ std::begin(container), std::end(container), std::forward<Selector>(selector) } };
        };
    }

    namespace impl
    {
        template <typename It1, typename It2, typename CSelector, typename RSelector>
        class select_many_iterator : public iterator_base<select_many_iterator<It1, It2, CSelector, RSelector>>
        {
        private:
            It1 m_begin1{}, m_end1{};
            It2 m_begin2{}, m_end2{};
            std::shared_ptr<CSelector> m_cselector{};
            std::shared_ptr<RSelector> m_rselector{};

            using container_type = std::remove_reference_t<decltype((*m_cselector)(*m_begin1))>;
            std::shared_ptr<container_type> m_container{};

            using result1_type = typename std::iterator_traits<It1>::pointer;
            result1_type m_result1{};
            using result_type = decltype((*m_rselector)(*m_result1, *m_begin2));
            std::optional<result_type> m_result{};

            void set_container()
            {
                if (m_begin1 != m_end1)
                {
                    m_result1 = &*m_begin1;
                    m_container = std::make_shared<container_type>((*m_cselector)(*m_result1));
                    m_begin2 = std::begin(*m_container);
                    m_end2 = std::end(*m_container);
                }
                else
                {
                    m_container = nullptr;
                    this->m_valid = false;
                }
            }

            void set_result()
            {
                if (this->m_valid)
                    m_result.emplace((*m_rselector)(*m_result1, *m_begin2));
            }

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = result_type;
            using difference_type = typename std::iterator_traits<It2>::difference_type;
            using pointer = const result_type*;
            using reference = const result_type&;

            select_many_iterator() {}
            select_many_iterator(It1 begin, It1 end, CSelector cselector, RSelector rselector)
                : iterator_base<select_many_iterator>(true), m_begin1(begin), m_end1(end),
                  m_cselector(std::make_shared<CSelector>(std::move(cselector))), m_rselector(std::make_shared<RSelector>(std::move(rselector)))
            {
                set_container();
                set_result();
            }
            select_many_iterator(const select_many_iterator& it) { *this = it; }
            select_many_iterator& operator=(const select_many_iterator& it) = default;

            reference operator*() const noexcept { return *m_result; }

            constexpr select_many_iterator& operator++()
            {
                ++m_begin2;
                if (m_begin2 == m_end2)
                {
                    ++m_begin1;
                    set_container();
                }
                set_result();
                return *this;
            }
        };
    } // namespace impl

    // Projects each element, flattens the resulting sequences into one sequence, and invokes a result selector function on each element therein.
    template <typename CSelector, typename RSelector>
    constexpr auto select_many(CSelector&& cselector, RSelector&& rselector)
    {
        return [&](auto&& container) {
            using It1 = decltype(std::begin(container));
            using It2 = decltype(std::begin(cselector(*std::begin(container))));
            return impl::iterable<impl::select_many_iterator<It1, It2, CSelector, RSelector>>{ impl::select_many_iterator<It1, It2, CSelector, RSelector>{ std::begin(container), std::end(container), std::forward<CSelector>(cselector), std::forward<RSelector>(rselector) } };
        };
    }

    //namespace impl
    //{
    //    template <typename Eter, typename Eter2, typename CSelector, typename RSelector>
    //    class select_many_index_enumerator
    //    {
    //    private:
    //        Eter m_eter;
    //        std::optional<remove_cref<decltype(*m_eter)>> m_eter_value;
    //        std::optional<Eter2> m_eter2;
    //        CSelector m_cselector;
    //        RSelector m_rselector;
    //        std::size_t m_index;

    //        constexpr void move_next()
    //        {
    //            if (m_eter)
    //            {
    //                // Use emplace to prevent an operator= bug of optional.
    //                m_eter_value.emplace(*m_eter);
    //                m_eter2.emplace(get_enumerator(m_cselector(*m_eter_value, m_index)));
    //                ++m_eter;
    //                ++m_index;
    //            }
    //            else
    //            {
    //                m_eter2 = std::nullopt;
    //            }
    //        }

    //    public:
    //        constexpr select_many_index_enumerator(Eter&& eter, CSelector&& cselector, RSelector&& rselector)
    //            : m_eter(std::forward<Eter>(eter)), m_cselector(cselector), m_rselector(rselector), m_index(0)
    //        {
    //            move_next();
    //        }

    //        constexpr operator bool() const { return *m_eter2 || m_eter; }
    //        constexpr select_many_index_enumerator& operator++()
    //        {
    //            ++*m_eter2;
    //            if (!*m_eter2)
    //            {
    //                move_next();
    //            }
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*() { return m_rselector(*m_eter_value, **m_eter2); }
    //    };
    //} // namespace impl

    //// Projects each element, flattens the resulting sequences into one sequence, and invokes a result selector function on each element therein.
    //template <typename CSelector, typename RSelector>
    //constexpr auto select_many_index(CSelector&& cselector, RSelector&& rselector)
    //{
    //    return [&](auto e) {
    //        using Eter = decltype(e.enumerator());
    //        using Eter2 = decltype(get_enumerator(cselector(*e.enumerator(), 0)));
    //        return enumerable(impl::select_many_index_enumerator<Eter, Eter2, CSelector, RSelector>(e.enumerator(), std::forward<CSelector>(cselector), std::forward<RSelector>(rselector)));
    //    };
    //}

    //namespace impl
    //{
    //    template <typename Eter>
    //    class skip_enumerator
    //    {
    //    private:
    //        Eter m_eter;

    //    public:
    //        constexpr skip_enumerator(Eter&& eter, std::size_t skipn) : m_eter(std::forward<Eter>(eter))
    //        {
    //            for (; m_eter && skipn; ++m_eter, --skipn)
    //                ;
    //        }

    //        constexpr operator bool() const { return m_eter; }
    //        constexpr skip_enumerator& operator++()
    //        {
    //            ++m_eter;
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*() { return *m_eter; }
    //    };
    //} // namespace impl

    //// Bypasses a specified number of elements and then returns the remaining elements.
    //constexpr auto skip(std::size_t skipn)
    //{
    //    return [=](auto e) {
    //        using Eter = decltype(e.enumerator());
    //        return enumerable(impl::skip_enumerator<Eter>(e.enumerator(), skipn));
    //    };
    //}

    //namespace impl
    //{
    //    template <typename Eter>
    //    class skip_while_enumerator
    //    {
    //    private:
    //        Eter m_eter;

    //    public:
    //        template <typename Pred>
    //        constexpr skip_while_enumerator(Eter&& eter, Pred&& pred) : m_eter(std::forward<Eter>(eter))
    //        {
    //            for (; m_eter && pred(*m_eter); ++m_eter)
    //                ;
    //        }

    //        constexpr operator bool() const { return m_eter; }
    //        constexpr skip_while_enumerator& operator++()
    //        {
    //            ++m_eter;
    //            return *this;
    //        }
    //        constexpr decltype(auto) operator*() { return *m_eter; }
    //    };
    //} // namespace impl

    //// Bypasses elements as long as a specified condition is true and then returns the remaining elements.
    //template <typename Pred>
    //constexpr auto skip_while(Pred&& pred)
    //{
    //    return [&](auto e) {
    //        using Eter = decltype(e.enumerator());
    //        return enumerable(impl::skip_while_enumerator<Eter>(e.enumerator(), std::forward<Pred>(pred)));
    //    };
    //}

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
