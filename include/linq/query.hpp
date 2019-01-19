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

namespace linq
{
    namespace impl
    {
        template <typename Eter, typename Pred>
        class where_enumerator
        {
        private:
            Eter m_eter;
            Pred m_pred;

            constexpr void move_next()
            {
                for (; m_eter; ++m_eter)
                {
                    if (m_pred(*m_eter))
                        break;
                }
            }

        public:
            constexpr where_enumerator(Eter&& eter, Pred&& pred) : m_eter(std::forward<Eter>(eter)), m_pred(pred) { move_next(); }

            constexpr operator bool() const { return m_eter; }
            constexpr where_enumerator& operator++()
            {
                ++m_eter;
                move_next();
                return *this;
            }
            constexpr decltype(auto) operator*() { return *m_eter; }
        };
    } // namespace impl

    // Filters an enumerable based on a predicate.
    template <typename Pred>
    constexpr auto where(Pred&& pred)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::where_enumerator<Eter, Pred>(e.enumerator(), std::forward<Pred>(pred)));
        };
    }

    namespace impl
    {
        template <typename Eter, typename Pred>
        class where_index_enumerator
        {
        private:
            Eter m_eter;
            Pred m_pred;
            std::size_t m_index;

            constexpr void move_next()
            {
                for (; m_eter; ++m_eter, ++m_index)
                {
                    if (m_pred(*m_eter, m_index))
                        break;
                }
            }

        public:
            constexpr where_index_enumerator(Eter&& eter, Pred&& pred) : m_eter(std::forward<Eter>(eter)), m_pred(pred), m_index(0) { move_next(); }

            constexpr operator bool() const { return m_eter; }
            constexpr where_index_enumerator& operator++()
            {
                ++m_eter;
                ++m_index;
                move_next();
                return *this;
            }
            constexpr decltype(auto) operator*() { return *m_eter; }
        };
    } // namespace impl

    template <typename Pred>
    constexpr auto where_index(Pred&& pred)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::where_index_enumerator<Eter, Pred>(e.enumerator(), std::forward<Pred>(pred)));
        };
    }

    namespace impl
    {
        template <typename Eter, typename Selector>
        class select_enumerator
        {
        private:
            Eter m_eter;
            Selector m_selector;

        public:
            constexpr select_enumerator(Eter&& eter, Selector&& selector) : m_eter(std::forward<Eter>(eter)), m_selector(selector) {}

            constexpr operator bool() const { return m_eter; }
            constexpr select_enumerator& operator++()
            {
                ++m_eter;
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_selector(*m_eter); }
        };
    } // namespace impl

    // Projects each element into a new form.
    template <typename Selector>
    constexpr auto select(Selector&& selector)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::select_enumerator<Eter, Selector>(e.enumerator(), std::forward<Selector>(selector)));
        };
    }

    namespace impl
    {
        template <typename Eter, typename Eter2, typename CSelector, typename RSelector>
        class select_many_enumerator
        {
        private:
            Eter m_eter;
            std::optional<Eter2> m_eter2;
            CSelector m_cselector;
            RSelector m_rselector;

            constexpr void move_next()
            {
                if (m_eter)
                {
                    // Use emplace to prevent a operator= bug of optional.
                    m_eter2.emplace(get_enumerator(m_cselector(*m_eter)));
                    ++m_eter;
                }
                else
                {
                    m_eter2 = std::nullopt;
                }
            }

        public:
            constexpr select_many_enumerator(Eter&& eter, CSelector&& cselector, RSelector&& rselector)
                : m_eter(std::forward<Eter>(eter)), m_cselector(cselector), m_rselector(rselector)
            {
                move_next();
            }

            constexpr operator bool() const { return *m_eter2 || m_eter; }
            constexpr select_many_enumerator& operator++()
            {
                ++*m_eter2;
                if (!*m_eter2)
                {
                    move_next();
                }
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_rselector(**m_eter2); }
        };
    } // namespace impl

    // Projects each element, flattens the resulting sequences into one sequence, and invokes a result selector function on each element therein.
    template <typename CSelector, typename RSelector>
    constexpr auto select_many(CSelector&& cselector, RSelector&& rselector)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            using Eter2 = decltype(get_enumerator(cselector(*e.enumerator())));
            return enumerable(impl::select_many_enumerator<Eter, Eter2, CSelector, RSelector>(e.enumerator(), std::forward<CSelector>(cselector), std::forward<RSelector>(rselector)));
        };
    }

    namespace impl
    {
        template <typename Eter>
        class skip_enumerator
        {
        private:
            Eter m_eter;

        public:
            constexpr skip_enumerator(Eter&& eter, std::size_t skipn) : m_eter(std::forward<Eter>(eter))
            {
                for (; m_eter && skipn; ++m_eter, --skipn)
                    ;
            }

            constexpr operator bool() const { return m_eter; }
            constexpr skip_enumerator& operator++()
            {
                ++m_eter;
                return *this;
            }
            constexpr decltype(auto) operator*() { return *m_eter; }
        };
    } // namespace impl

    // Bypasses a specified number of elements and then returns the remaining elements.
    constexpr auto skip(std::size_t skipn)
    {
        return [=](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::skip_enumerator<Eter>(e.enumerator(), skipn));
        };
    }

    namespace impl
    {
        template <typename Eter>
        class skip_while_enumerator
        {
        private:
            Eter m_eter;

        public:
            template <typename Pred>
            constexpr skip_while_enumerator(Eter&& eter, Pred&& pred) : m_eter(std::forward<Eter>(eter))
            {
                for (; m_eter && pred(*m_eter); ++m_eter)
                    ;
            }

            constexpr operator bool() const { return m_eter; }
            constexpr skip_while_enumerator& operator++()
            {
                ++m_eter;
                return *this;
            }
            constexpr decltype(auto) operator*() { return *m_eter; }
        };
    } // namespace impl

    // Bypasses elements as long as a specified condition is true and then returns the remaining elements.
    template <typename Pred>
    constexpr auto skip_while(Pred&& pred)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::skip_while_enumerator<Eter>(e.enumerator(), std::forward<Pred>(pred)));
        };
    }

    namespace impl
    {
        template <typename Eter>
        class take_enumerator
        {
        private:
            Eter m_eter;
            // skip doesn't need the number because it just skipped.
            std::size_t m_taken;

        public:
            constexpr take_enumerator(Eter&& eter, std::size_t taken) : m_eter(std::forward<Eter>(eter)), m_taken(taken) {}

            constexpr operator bool() const { return m_eter && m_taken; }
            constexpr take_enumerator& operator++()
            {
                ++m_eter;
                --m_taken;
                return *this;
            }
            constexpr decltype(auto) operator*() { return *m_eter; }
        };
    } // namespace impl

    // Returns a specified number of contiguous elements from the start.
    constexpr auto take(std::size_t taken)
    {
        return [=](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::take_enumerator<Eter>(e.enumerator(), taken));
        };
    }

    namespace impl
    {
        template <typename Eter, typename Pred>
        class take_while_enumerator
        {
        private:
            Eter m_eter;
            Pred m_pred;

        public:
            constexpr take_while_enumerator(Eter&& eter, Pred&& pred) : m_eter(std::forward<Eter>(eter)), m_pred(pred) {}

            constexpr operator bool() { return m_eter && m_pred(*m_eter); }
            constexpr take_while_enumerator& operator++()
            {
                ++m_eter;
                return *this;
            }
            constexpr decltype(auto) operator*() { return *m_eter; }
        };
    } // namespace impl

    // Returns elements as long as a specified condition is true.
    template <typename Pred>
    constexpr auto take_while(Pred&& pred)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::take_while_enumerator<Eter, Pred>(e.enumerator(), std::forward<Pred>(pred)));
        };
    }

    namespace impl
    {
        template <typename Eter1, typename Eter2>
        class concat_enumerator
        {
        private:
            Eter1 m_eter1;
            Eter2 m_eter2;

        public:
            constexpr concat_enumerator(Eter1&& eter1, Eter2&& eter2) : m_eter1(std::forward<Eter1>(eter1)), m_eter2(std::forward<Eter2>(eter2)) {}

            constexpr operator bool() const { return m_eter1 || m_eter2; }
            constexpr concat_enumerator& operator++()
            {
                if (m_eter1)
                    ++m_eter1;
                else
                    ++m_eter2;
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_eter1 ? *m_eter1 : *m_eter2; }
        };
    } // namespace impl

    // Concatenates two enumerable.
    template <typename E2>
    constexpr auto concat(E2&& e2)
    {
        return [&](auto e) {
            using Eter1 = decltype(e.enumerator());
            using Eter2 = decltype(get_enumerator(e2));
            static_assert(std::is_same_v<decltype(*e.enumerator()), decltype(*get_enumerator(e2))>, "The return type of the two enumerable should be the same.");
            return enumerable(impl::concat_enumerator<Eter1, Eter2>(e.enumerator(), get_enumerator(e2)));
        };
    }

    namespace impl
    {
        template <typename Eter1, typename Eter2, typename Selector>
        class zip_enumerator
        {
        private:
            Eter1 m_eter1;
            Eter2 m_eter2;
            Selector m_selector;

        public:
            constexpr zip_enumerator(Eter1&& eter1, Eter2&& eter2, Selector&& selector) : m_eter1(std::forward<Eter1>(eter1)), m_eter2(std::forward<Eter2>(eter2)), m_selector(selector) {}

            constexpr operator bool() const { return m_eter1 && m_eter2; }
            constexpr zip_enumerator& operator++()
            {
                ++m_eter1;
                ++m_eter2;
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_selector(*m_eter1, *m_eter2); }
        };
    } // namespace impl

    // Applies a specified function to the corresponding elements of two enumerable, producing an enumerable of the results.
    template <typename E2, typename Selector>
    constexpr auto zip(E2&& e2, Selector&& selector)
    {
        return [&](auto e) {
            using Eter1 = decltype(e.enumerator());
            using Eter2 = decltype(get_enumerator(std::forward<E2>(e2)));
            return enumerable(impl::zip_enumerator<Eter1, Eter2, Selector>(e.enumerator(), get_enumerator(std::forward<E2>(e2)), std::forward<Selector>(selector)));
        };
    }

    namespace impl
    {
        template <typename Eter, typename TTo>
        class cast_enumerator
        {
        private:
            Eter m_eter;

        public:
            cast_enumerator(Eter&& eter) : m_eter(std::forward<Eter>(eter)) {}

            constexpr operator bool() const { return m_eter; }
            constexpr cast_enumerator& operator++()
            {
                ++m_eter;
                return *this;
            }
            constexpr TTo operator*() { return (TTo)*m_eter; }
        };
    } // namespace impl

    // Applies C-style cast to the elements.
    template <typename TTo>
    constexpr auto cast()
    {
        return [](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::cast_enumerator<Eter, TTo>(e.enumerator()));
        };
    }
} // namespace linq

#endif // !LINQ_QUERY_HPP
