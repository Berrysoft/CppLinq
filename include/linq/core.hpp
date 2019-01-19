/**CppLinq core.hpp
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
#ifndef LINQ_CORE_HPP
#define LINQ_CORE_HPP

#include <iterator>
#include <optional>

namespace linq
{
    namespace impl
    {
        // An enumerator for an STL container.
        // Like IEnumerator in .NET.
        // To enumerate an enumerator named eter, do:
        // for( ; eter; ++eter)
        // {
        //     // Do with *eter
        // }
        template <typename Container, typename Iter = decltype(std::begin(std::declval<Container>()))>
        class enumerator
        {
        private:
            Container m_container;
            Iter m_begin, m_end;

        public:
            constexpr enumerator(Container&& container) : m_container(std::forward<Container>(container)), m_begin(std::begin(m_container)), m_end(std::end(m_container)) {}

            constexpr Container container() { return m_container; }

            constexpr operator bool() const { return m_begin != m_end; }
            constexpr enumerator& operator++()
            {
                ++m_begin;
                return *this;
            }
            constexpr decltype(auto) operator*() { return *m_begin; }
        };

        template <typename Iter>
        class iter_enumerator
        {
        private:
            Iter m_begin, m_end;

        public:
            constexpr iter_enumerator(Iter&& begin, Iter&& end) : m_begin(begin), m_end(end) {}

            constexpr operator bool() const { return m_begin != m_end; }
            constexpr iter_enumerator& operator++()
            {
                ++m_begin;
                return *this;
            }
            constexpr decltype(auto) operator*() { return *m_begin; }
        };
    } // namespace impl

    // An STL iterator adapter for enumerator-like class.
    template <typename Eter>
    class enumerator_iterator
    {
    private:
        std::optional<Eter> m_eter;

    public:
        constexpr enumerator_iterator() : m_eter(std::nullopt) {}
        constexpr enumerator_iterator(Eter&& eter) : m_eter(std::forward<Eter>(eter)) {}

        constexpr Eter enumerator() { return *m_eter; }

        constexpr decltype(auto) operator*() { return *(*m_eter); }
        constexpr enumerator_iterator& operator++()
        {
            ++(*m_eter);
            return *this;
        }

        friend constexpr bool operator==(enumerator_iterator& it1, enumerator_iterator& it2) { return !operator!=(it1, it2); }
        friend constexpr bool operator!=(enumerator_iterator& it1, enumerator_iterator& it2) { return (it1.m_eter && *it1.m_eter) || (it2.m_eter && *it2.m_eter); }
    };

    // An STL container adapter for enumerator-like class.
    template <typename Eter>
    class enumerable
    {
    private:
        enumerator_iterator<Eter> m_iter;

    public:
        constexpr Eter enumerator() { return m_iter.enumerator(); }

        constexpr auto begin() { return m_iter; }
        constexpr auto end() { return enumerator_iterator<Eter>(); }

        constexpr enumerable() = default;

        constexpr enumerable(Eter&& eter) : m_iter(std::forward<Eter>(eter)) {}
    };

    // SFINAE
    template <typename T, typename = void>
    inline constexpr bool is_enumerable_v{ false };

    template <typename T>
    inline constexpr bool is_enumerable_v<T, std::void_t<decltype(std::declval<T>().enumerator())>>{ true };

    template <typename T>
    using remove_cref = std::remove_const_t<std::remove_reference_t<T>>;

    // Help functions for enumerator and enumerable.
    template <typename Enumerable, typename = std::enable_if_t<is_enumerable_v<Enumerable>>>
    constexpr decltype(auto) get_enumerator(Enumerable&& e)
    {
        return e.enumerator();
    }

    template <typename Container, typename = std::enable_if_t<!is_enumerable_v<Container>>, typename = decltype(std::begin(std::declval<Container>())), typename = decltype(std::end(std::declval<Container>()))>
    constexpr decltype(auto) get_enumerator(Container&& c)
    {
        return impl::enumerator<Container>(std::forward<Container>(c));
    }

    template <typename Container>
    constexpr auto get_enumerable(Container&& container)
    {
        return enumerable(get_enumerator(std::forward<Container>(container)));
    }

    template <typename Iter>
    constexpr auto get_enumerable(Iter&& begin, Iter&& end)
    {
        return enumerable(impl::iter_enumerator(std::forward<Iter>(begin), std::forward<Iter>(end)));
    }

    // Combine enumerable and query lambdas.
    template <typename Enumerable, typename Query, typename = std::enable_if_t<is_enumerable_v<Enumerable>>>
    constexpr decltype(auto) operator>>(Enumerable&& e, Query&& q)
    {
        return std::forward<Query>(q)(std::forward<Enumerable>(e));
    }

    template <typename Container, typename Query, typename = std::enable_if_t<!is_enumerable_v<Container>>, typename = decltype(std::begin(std::declval<Container>())), typename = decltype(std::end(std::declval<Container>()))>
    constexpr decltype(auto) operator>>(Container&& c, Query&& q)
    {
        return std::forward<Query>(q)(get_enumerable(std::forward<Container>(c)));
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

    // Ranges an interger in [begin, end).
    template <typename Int>
    constexpr auto range(Int begin, Int end)
    {
        return enumerable(impl::range_enumerator<Int>(std::move(begin), std::move(end)));
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
            repeat_enumerator(T&& element, std::size_t num) : m_element(std::forward<T>(element)), m_num(num) {}

            constexpr operator bool() const { return m_num; }
            constexpr repeat_enumerator& operator++()
            {
                --m_num;
                return *this;
            }
            constexpr T operator*() { return m_element; }
        };
    } // namespace impl

    // Repeats an element several times.
    template <typename T>
    constexpr auto repeat(T&& element, std::size_t num)
    {
        return enumerable(impl::repeat_enumerator<T>(std::forward<T>(element), num));
    }

    namespace impl
    {
        template <typename T, typename Eter>
        class append_enumerator
        {
        private:
            std::optional<T> m_value;
            Eter m_eter;

        public:
            constexpr append_enumerator(T&& value, Eter&& eter) : m_value(std::forward<T>(value)), m_eter(std::forward<Eter>(eter)) {}

            constexpr operator bool() const { return m_eter || m_value; }
            constexpr append_enumerator& operator++()
            {
                if (m_eter)
                    ++m_eter;
                else
                    m_value = std::nullopt;
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_eter ? *m_eter : *m_value; }
        };
    } // namespace impl

    // Appends an element to the enumerable.
    template <typename T>
    constexpr auto append(T&& value)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::append_enumerator<T, Eter>(std::forward<T>(value), e.enumerator()));
        };
    }

    namespace impl
    {
        template <typename T, typename Eter>
        class prepend_enumerator
        {
        private:
            std::optional<T> m_value;
            Eter m_eter;

        public:
            constexpr prepend_enumerator(T&& value, Eter&& eter) : m_value(std::forward<T>(value)), m_eter(std::forward<Eter>(eter)) {}

            constexpr operator bool() const { return m_value || m_eter; }
            constexpr prepend_enumerator& operator++()
            {
                if (m_value)
                    m_value = std::nullopt;
                else
                    ++m_eter;
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_value ? *m_value : *m_eter; }
        };
    } // namespace impl

    // Prepends an element to the enumerable.
    template <typename T>
    constexpr auto prepend(T&& value)
    {
        return [&](auto e) {
            using Eter = decltype(e.enumerator());
            return enumerable(impl::prepend_enumerator<T, Eter>(std::forward<T>(value), e.enumerator()));
        };
    }

    // Determines whether the two enumerable are equal.
    template <typename E2>
    constexpr auto equals(E2&& e2)
    {
        return [&](auto e) {
            auto eter1{ e.enumerator() };
            auto eter2{ get_enumerator(std::forward<E2>(e2)) };
            for (; eter1 && eter2; ++eter1, ++eter2)
            {
                if (*eter1 != *eter2)
                    return false;
            }
            return !eter1 && !eter2;
        };
    }
} // namespace linq

#endif // !LINQ_CORE_HPP
