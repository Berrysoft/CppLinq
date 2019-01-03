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
            constexpr where_enumerator(Eter&& eter, Pred&& pred) : m_eter(eter), m_pred(pred) { move_next(); }

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

    template <typename Pred>
    constexpr auto where(Pred&& pred)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable<impl::where_enumerator<Eter, Pred>>(impl::where_enumerator<Eter, Pred>(e.enumerator(), std::forward<Pred>(pred)));
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
            constexpr select_enumerator(Eter&& eter, Selector&& selector) : m_eter(eter), m_selector(selector) {}

            constexpr operator bool() const { return m_eter; }
            constexpr select_enumerator& operator++()
            {
                ++m_eter;
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_selector(*m_eter); }
        };
    } // namespace impl

    template <typename Selector>
    constexpr auto select(Selector&& selector)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable<impl::select_enumerator<Eter, Selector>>(impl::select_enumerator<Eter, Selector>(e.enumerator(), std::forward<Selector>(selector)));
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
            constexpr skip_enumerator(Eter&& eter, std::size_t skipn) : m_eter(eter)
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

    constexpr auto skip(std::size_t skipn)
    {
        return [=](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable<impl::skip_enumerator<Eter>>(impl::skip_enumerator<Eter>(e.enumerator(), skipn));
        };
    }

    namespace impl
    {
        template <typename Eter>
        class take_enumerator
        {
        private:
            Eter m_eter;
            std::size_t m_taken;

        public:
            constexpr take_enumerator(Eter&& eter, std::size_t taken) : m_eter(eter), m_taken(taken) {}

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

    constexpr auto take(std::size_t taken)
    {
        return [=](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable<impl::take_enumerator<Eter>>(impl::take_enumerator<Eter>(e.enumerator(), taken));
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
            constexpr concat_enumerator(Eter1&& eter1, Eter2&& eter2) : m_eter1(eter1), m_eter2(eter2) {}

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

    template <typename E2>
    constexpr auto concat(E2&& e2)
    {
        return [&](auto e) {
            using Eter1 = decltype(e.enumerator());
            using Eter2 = decltype(e2.enumerator());
            static_assert(std::is_same_v<decltype(*e.enumerator()), decltype(*e2.enumerator())>, "The return type of the two enumerable should be the same.");
            return enumerable<impl::concat_enumerator<Eter1, Eter2>>(impl::concat_enumerator<Eter1, Eter2>(e.enumerator(), e2.enumerator()));
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
            constexpr zip_enumerator(Eter1&& eter1, Eter2&& eter2, Selector&& selector) : m_eter1(eter1), m_eter2(eter2), m_selector(selector) {}

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

    template <typename E2, typename Selector>
    constexpr auto zip(E2&& e2, Selector&& selector)
    {
        return [&](auto e) {
            using Eter1 = decltype(e.enumerator());
            using Eter2 = decltype(e2.enumerator());
            return enumerable<impl::zip_enumerator<Eter1, Eter2, Selector>>(impl::zip_enumerator<Eter1, Eter2, Selector>(e.enumerator(), e2.enumerator(), std::forward<Selector>(selector)));
        };
    }
} // namespace linq

#endif // !LINQ_QUERY_HPP
