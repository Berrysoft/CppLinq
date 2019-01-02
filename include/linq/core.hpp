#ifndef LINQ_CORE_HPP
#define LINQ_CORE_HPP

#include <iterator>
#include <optional>

namespace linq
{
    namespace impl
    {
        template <typename Iter>
        class enumerator
        {
        private:
            Iter m_begin, m_end;

        public:
            constexpr enumerator(Iter&& begin, Iter&& end) : m_begin(std::forward<Iter>(begin)), m_end(std::forward<Iter>(end)) {}

            constexpr operator bool() const { return m_begin != m_end; }
            constexpr enumerator& operator++()
            {
                ++m_begin;
                return *this;
            }

            constexpr decltype(auto) operator*() { return *m_begin; }
        };

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

        template <typename Int>
        class range_enumerator
        {
        private:
            Int m_begin, m_end;

        public:
            constexpr range_enumerator(Int&& begin, Int&& end) : m_begin(begin), m_end(end) {}

            constexpr operator bool() const { return m_begin != m_end; }
            constexpr range_enumerator& operator++()
            {
                ++m_begin;
                return *this;
            }
            constexpr Int operator*() { return m_begin; }
        };

        template <typename Eter>
        class enumerator_iterator
        {
        private:
            std::optional<Eter> m_eter;

        public:
            constexpr enumerator_iterator() : m_eter(std::nullopt) {}
            constexpr enumerator_iterator(Eter eter) : m_eter(eter) {}

            constexpr decltype(auto) operator*() { return **m_eter; }
            constexpr enumerator_iterator& operator++()
            {
                ++(*m_eter);
                return *this;
            }

            friend constexpr bool operator==(const enumerator_iterator& it1, const enumerator_iterator& it2) { return !operator!=(it1, it2); }
            friend constexpr bool operator!=(const enumerator_iterator& it1, const enumerator_iterator& it2) { return (it1.m_eter && *it1.m_eter) || (it2.m_eter && *it2.m_eter); }
        };
    } // namespace impl

    template <typename Eter>
    class enumerable
    {
    private:
        Eter m_eter;

    public:
        constexpr auto begin() { return impl::enumerator_iterator(m_eter); }
        constexpr auto end() { return impl::enumerator_iterator<Eter>(); }

        constexpr enumerable() = default;

        constexpr enumerable(Eter&& eter) : m_eter(eter) {}

        template <typename Container>
        constexpr enumerable(Container&& c) : m_eter(std::begin(c), std::end(c))
        {
        }

        template <typename Iter>
        constexpr enumerable(Iter&& begin, Iter&& end) : m_eter(std::forward<Iter>(begin), std::forward<Iter>(end))
        {
        }

        template <typename Pred>
        constexpr auto where(Pred&& pred)
        {
            return enumerable<impl::where_enumerator<Eter, Pred>>(impl::where_enumerator<Eter, Pred>(std::forward<Eter>(m_eter), std::forward<Pred>(pred)));
        }
        template <typename Selector>
        constexpr auto select(Selector&& selector)
        {
            return enumerable<impl::select_enumerator<Eter, Selector>>(impl::select_enumerator<Eter, Selector>(std::forward<Eter>(m_eter), std::forward<Selector>(selector)));
        }
    };

    template <typename Container>
    enumerable(Container&& c)->enumerable<impl::enumerator<decltype(std::begin(c))>>;

    template <typename Iter>
    enumerable(Iter&& begin, Iter&& end)->enumerable<impl::enumerator<Iter>>;

    template <typename Int>
    constexpr auto range(Int begin, Int end)
    {
        return enumerable<impl::range_enumerator<Int>>(impl::range_enumerator<Int>(std::move(begin), std::move(end)));
    }
} // namespace linq

#endif // !LINQ_CORE_HPP
