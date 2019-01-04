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
    } // namespace impl

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

        friend constexpr bool operator==(enumerator_iterator& it1, enumerator_iterator& it2) { return !operator!=(it1, it2); }
        friend constexpr bool operator!=(enumerator_iterator& it1, enumerator_iterator& it2) { return (it1.m_eter && *it1.m_eter) || (it2.m_eter && *it2.m_eter); }
    };

    template <typename Eter>
    class enumerable
    {
    private:
        Eter m_eter;

    public:
        constexpr Eter enumerator() const { return m_eter; }

        constexpr auto begin() const { return enumerator_iterator(m_eter); }
        constexpr auto end() const { return enumerator_iterator<Eter>(); }

        constexpr enumerable() = default;

        constexpr enumerable(Eter&& eter) : m_eter(eter) {}

        template <typename Iter>
        constexpr enumerable(Iter&& begin, Iter&& end) : m_eter(std::forward<Iter>(begin), std::forward<Iter>(end))
        {
        }
    };

    template <typename Iter>
    enumerable(Iter&& begin, Iter&& end)->enumerable<impl::enumerator<Iter>>;

    template <typename T, typename = void>
    inline constexpr bool is_enumerable_v{ false };

    template <typename T>
    inline constexpr bool is_enumerable_v<T, std::void_t<decltype(std::declval<T>().enumerator())>>{ true };

    template <typename Container>
    constexpr auto get_enumerable(std::remove_reference_t<Container>& container)
    {
        return enumerable(impl::enumerator(std::begin(container), std::end(container)));
    }

    template <typename Enumerable, typename = std::enable_if_t<is_enumerable_v<Enumerable>>>
    constexpr decltype(auto) get_enumerator(Enumerable&& e)
    {
        return e.enumerator();
    }

    template <typename Container, typename = std::enable_if_t<!is_enumerable_v<Container>>, typename = decltype(std::begin(std::declval<Container>())), typename = decltype(std::end(std::declval<Container>()))>
    constexpr decltype(auto) get_enumerator(Container&& c)
    {
        return get_enumerable<Container>(std::forward<Container>(c)).enumerator();
    }

    template <typename Enumerable, typename Query, typename = std::enable_if_t<is_enumerable_v<Enumerable>>>
    constexpr decltype(auto) operator>>(Enumerable&& e, Query&& q)
    {
        return std::forward<Query>(q)(std::forward<Enumerable>(e));
    }

    template <typename Container, typename Query, typename = std::enable_if_t<!is_enumerable_v<Container>>, typename = decltype(std::begin(std::declval<Container>())), typename = decltype(std::end(std::declval<Container>()))>
    constexpr decltype(auto) operator>>(Container&& c, Query&& q)
    {
        return std::forward<Query>(q)(get_enumerable<Container>(std::forward<Container>(c)));
    }

    namespace impl
    {
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
    } // namespace impl

    template <typename Int>
    constexpr auto range(Int begin, Int end)
    {
        return enumerable<impl::range_enumerator<Int>>(impl::range_enumerator<Int>(std::move(begin), std::move(end)));
    }

    namespace impl
    {
        template <typename T>
        class repeat_enumerator
        {
        private:
            T m_element;
            std::size_t m_num;

        public:
            repeat_enumerator(T&& element, std::size_t num) : m_element(element), m_num(num) {}

            constexpr operator bool() const { return m_num; }
            constexpr repeat_enumerator& operator++()
            {
                --m_num;
                return *this;
            }
            constexpr T operator*() { return m_element; }
        };
    } // namespace impl

    template <typename T>
    constexpr auto repeat(T&& element, std::size_t num)
    {
        return enumerable<impl::repeat_enumerator<T>>(impl::repeat_enumerator<T>(std::forward<T>(element), num));
    }
} // namespace linq

#endif // !LINQ_CORE_HPP
