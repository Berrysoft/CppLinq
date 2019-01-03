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

    inline auto skip(std::size_t skipn)
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

    inline auto take(std::size_t taken)
    {
        return [=](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable<impl::take_enumerator<Eter>>(impl::take_enumerator<Eter>(e.enumerator(), taken));
        };
    }

    inline auto _allways_true()
    {
        return [](auto) { return true; };
    }

    template <typename Pred = decltype(_allways_true())>
    constexpr auto count(Pred&& pred = _allways_true())
    {
        return [&](auto e) {
            std::size_t result = 0;
            for (auto item : e)
            {
                if (pred(item))
                    ++result;
            }
            return result;
        };
    }
} // namespace linq

#endif // !LINQ_QUERY_HPP
