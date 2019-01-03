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
        constexpr Eter enumerator() const { return m_eter; }

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
    };

    template <typename Container>
    enumerable(Container&& c)->enumerable<impl::enumerator<decltype(std::begin(c))>>;

    template <typename Iter>
    enumerable(Iter&& begin, Iter&& end)->enumerable<impl::enumerator<Iter>>;

    template <typename T, typename = void>
    inline constexpr bool is_enumerable_v = false;

    template <typename T>
    inline constexpr bool is_enumerable_v<T, std::void_t<decltype(std::declval<T>().enumerator())>> = true;

    template <typename Enumerable, typename Query, typename = std::enable_if_t<is_enumerable_v<Enumerable>>>
    constexpr decltype(auto) operator>>(Enumerable&& e, Query&& q)
    {
        return q(std::forward<Enumerable>(e));
    }

    template <typename Container, typename Query, typename = std::enable_if_t<!is_enumerable_v<Container>>, typename = decltype(std::begin(std::declval<Container>())), typename = decltype(std::end(std::declval<Container>()))>
    constexpr decltype(auto) operator>>(Container&& c, Query&& q)
    {
        return q(enumerable(std::forward<Container>(c)));
    }

    template <typename Int>
    constexpr auto range(Int begin, Int end)
    {
        return enumerable<impl::range_enumerator<Int>>(impl::range_enumerator<Int>(std::move(begin), std::move(end)));
    }
} // namespace linq

#endif // !LINQ_CORE_HPP
