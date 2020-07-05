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
        class iterator_base
        {
        protected:
            bool m_valid;

        public:
            constexpr iterator_base(bool valid = false) noexcept : m_valid(valid) {}
            iterator_base(const iterator_base&) noexcept = default;
            iterator_base& operator=(const iterator_base&) noexcept = default;

            void swap(iterator_base& it) noexcept { std::swap(m_valid, it.m_valid); }

            constexpr operator bool() const noexcept { return m_valid; }
        };

        template <typename It>
        class iterable
        {
        private:
            It m_begin, m_end;

        public:
            iterable() noexcept(std::is_nothrow_constructible_v<It>) : m_begin(), m_end() {}
            iterable(It&& begin) noexcept(std::is_nothrow_move_constructible_v<It>&& std::is_nothrow_constructible_v<It>)
                : m_begin(std::move(begin)), m_end()
            {
            }

            It begin() const noexcept(std::is_nothrow_copy_constructible_v<It>) { return m_begin; }
            It end() const noexcept(std::is_nothrow_copy_constructible_v<It>) { return m_end; }
        };
    } // namespace impl

    template <typename Container, typename Query, typename = decltype(std::begin(std::declval<Container>())), typename = decltype(std::end(std::declval<Container>()))>
    constexpr decltype(auto) operator>>(Container&& container, Query&& query)
    {
        return std::forward<Query>(query)(std::forward<Container>(container));
    }

    namespace impl
    {
        template <typename T>
        struct increase
        {
            constexpr T operator()(T value) noexcept(noexcept(++value)) { return ++value; }
        };

        template <typename T, typename Func>
        class range_iterator : public iterator_base
        {
        private:
            T m_current, m_end;
            std::optional<Func> m_func;

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using difference_type = std::intptr_t;
            using pointer = const T*;
            using reference = const T&;

            constexpr range_iterator() noexcept(std::is_nothrow_constructible_v<T>&& std::is_nothrow_constructible_v<Func>)
                : m_current(), m_end(), m_func(std::nullopt) {}
            constexpr range_iterator(T begin, T end, Func func) noexcept(std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_constructible_v<Func>)
                : iterator_base(true), m_current(std::move(begin)), m_end(std::move(end)), m_func(std::move(func)) {}
            range_iterator(const range_iterator&) noexcept(std::is_nothrow_copy_constructible_v<T>&& std::is_nothrow_copy_constructible_v<Func>) = default;
            range_iterator& operator=(const range_iterator&) noexcept(std::is_nothrow_copy_assignable_v<T>&& std::is_nothrow_copy_assignable_v<Func>) = default;

            void swap(range_iterator& it) noexcept(std::is_nothrow_swappable_v<T>&& std::is_nothrow_swappable_v<Func>)
            {
                iterator_base::swap(it);
                std::swap(m_current, it.m_current);
                std::swap(m_end, it.m_end);
                std::swap(m_func, it.m_func);
            }

            reference operator*() const noexcept { return m_current; }
            pointer operator->() const noexcept { return &m_current; }

            constexpr bool operator==(const range_iterator& it) const noexcept
            {
                if (this->m_valid && it.m_valid)
                {
                    return m_current == it.m_current && m_end == it.m_end;
                }
                else
                {
                    return !this->m_valid && !it.m_valid;
                }
            }
            constexpr bool operator!=(const range_iterator& it) const noexcept { return !(*this == it); }

            constexpr range_iterator& operator++()
            {
                m_current = (*m_func)(m_current);
                if (m_current >= m_end) this->m_valid = false;
                return *this;
            }
            range_iterator operator++(int)
            {
                range_iterator it = *this;
                operator++();
                return it;
            }
        };
    } // namespace impl

    template <typename T, typename F = impl::increase<T>>
    constexpr auto range(T begin, T end, F func = {})
    {
        return impl::iterable<impl::range_iterator<T, F>>{ impl::range_iterator<T, F>{ std::move(begin), std::move(end), std::move(func) } };
    }

    namespace impl
    {
        template <typename T>
        class repeat_iterator : public iterator_base
        {
        private:
            T m_value;
            std::size_t m_count;

        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = T;
            using difference_type = std::intptr_t;
            using pointer = const T*;
            using reference = const T&;

            constexpr repeat_iterator() noexcept(std::is_nothrow_constructible_v<T>) : m_value(), m_count(0) {}
            constexpr repeat_iterator(T value, std::size_t count) noexcept(std::is_nothrow_move_constructible_v<T>)
                : iterator_base(true), m_value(std::move(value)), m_count(count) {}
            repeat_iterator(const repeat_iterator&) = default;
            repeat_iterator& operator=(const repeat_iterator&) = default;

            void swap(repeat_iterator& it) noexcept(std::is_nothrow_swappable_v<T>)
            {
                iterator_base::swap(it);
                std::swap(m_value, it.m_value);
                std::swap(m_count, it.m_count);
            }

            reference operator*() const noexcept { return m_value; }
            pointer operator->() const noexcept { return &m_value; }

            constexpr bool operator==(const repeat_iterator& it) const noexcept
            {
                if (this->m_valid && it.m_valid)
                {
                    return m_value == it.m_value && m_count == it.m_count;
                }
                else
                {
                    return !this->m_valid && !it.m_valid;
                }
            }
            constexpr bool operator!=(const repeat_iterator& it) const noexcept { return !(*this == it); }

            constexpr repeat_iterator& operator++()
            {
                --m_count;
                if (m_count == 0) this->m_valid = false;
                return *this;
            }
            repeat_iterator operator++(int)
            {
                repeat_iterator it = *this;
                operator++();
                return it;
            }

            constexpr repeat_iterator& operator--()
            {
                ++m_count;
                return *this;
            }
            repeat_iterator operator--(int)
            {
                repeat_iterator it = *this;
                operator--();
                return it;
            }
        };
    } // namespace impl

    template <typename T>
    constexpr auto repeat(T value, std::size_t count)
    {
        return impl::iterable<impl::repeat_iterator<T>>{ impl::repeat_iterator<T>(std::move(value), count) };
    }

    namespace impl
    {
        template <typename T, typename It>
        class append_iterator : public iterator_base
        {
        private:
            std::optional<T> m_value;
            It m_begin, m_end;

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using difference_type = std::intptr_t;
            using pointer = const T*;
            using reference = const T&;

            constexpr append_iterator() : m_value(std::nullopt), m_begin(), m_end() {}
            constexpr append_iterator(T value, It begin, It end) : iterator_base(true), m_value(std::move(value)), m_begin(begin), m_end(end) {}
            append_iterator(const append_iterator&) = default;
            append_iterator& operator=(const append_iterator&) = default;

            void swap(append_iterator& it)
            {
                iterator_base::swap(it);
                std::swap(m_value, it.m_value);
                std::swap(m_begin, it.m_begin);
                std::swap(m_end, it.m_end);
            }

            reference operator*() const noexcept
            {
                if (m_begin != m_end)
                    return *m_begin;
                else
                    return *m_value;
            }
            pointer operator->() const noexcept { return &operator*(); }

            constexpr bool operator==(const append_iterator& it) const noexcept
            {
                if (this->m_valid && it.m_valid)
                {
                    return m_value == it.m_value && m_begin == it.m_begin && m_end == it.m_end;
                }
                else
                {
                    return !this->m_valid && !it.m_valid;
                }
            }
            constexpr bool operator!=(const append_iterator& it) const noexcept { return !(*this == it); }

            constexpr append_iterator& operator++()
            {
                if (m_begin != m_end)
                    ++m_begin;
                else
                    this->m_valid = false;
                return *this;
            }
            append_iterator operator++(int)
            {
                append_iterator it = *this;
                operator++();
                return it;
            }
        };
    } // namespace impl

    // Appends an element to the enumerable.
    template <typename T>
    constexpr auto append(T&& value)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable<impl::append_iterator<T, It>>{ impl::append_iterator<T, It>{ std::forward<T>(value), std::begin(container), std::end(container) } };
        };
    }

    namespace impl
    {
        template <typename T, typename It>
        class prepend_iterator : public iterator_base
        {
        private:
            std::optional<T> m_value;
            It m_begin, m_end;

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using difference_type = std::intptr_t;
            using pointer = const T*;
            using reference = const T&;

            constexpr prepend_iterator() : m_value(std::nullopt), m_begin(), m_end() {}
            constexpr prepend_iterator(T value, It begin, It end) : iterator_base(true), m_value(std::move(value)), m_begin(begin), m_end(end) {}
            prepend_iterator(const prepend_iterator&) = default;
            prepend_iterator& operator=(const prepend_iterator&) = default;

            void swap(prepend_iterator& it)
            {
                iterator_base::swap(it);
                std::swap(m_value, it.m_value);
                std::swap(m_begin, it.m_begin);
                std::swap(m_end, it.m_end);
            }

            reference operator*() const noexcept
            {
                if (m_value)
                    return *m_value;
                else
                    return *m_begin;
            }
            pointer operator->() const noexcept { return &operator*(); }

            constexpr bool operator==(const prepend_iterator& it) const noexcept
            {
                if (this->m_valid && it.m_valid)
                {
                    return m_value == it.m_value && m_begin == it.m_begin && m_end == it.m_end;
                }
                else
                {
                    return !this->m_valid && !it.m_valid;
                }
            }
            constexpr bool operator!=(const prepend_iterator& it) const noexcept { return !(*this == it); }

            constexpr prepend_iterator& operator++()
            {
                if (m_value)
                    m_value = std::nullopt;
                else
                {
                    ++m_begin;
                    if (m_begin == m_end) this->m_valid = false;
                }
                return *this;
            }
            prepend_iterator operator++(int)
            {
                prepend_iterator it = *this;
                operator++();
                return it;
            }
        };
    } // namespace impl

    // Prepends an element to the enumerable.
    template <typename T>
    constexpr auto prepend(T&& value)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable<impl::prepend_iterator<T, It>>{ impl::prepend_iterator<T, It>{ std::forward<T>(value), std::begin(container), std::end(container) } };
        };
    }

    namespace impl
    {
        template <typename It1, typename It2>
        class concat_iterator : public iterator_base
        {
        private:
            It1 m_begin1, m_end1;
            It2 m_begin2, m_end2;

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = std::common_type_t<std::remove_reference_t<decltype(*std::declval<It1>())>, std::remove_reference_t<decltype(*std::declval<It2>())>>;
            using difference_type = std::intptr_t;
            using pointer = const value_type*;
            using reference = const value_type&;

            concat_iterator() : m_begin1(), m_end1(), m_begin2(), m_end2() {}
            concat_iterator(It1 begin1, It1 end1, It2 begin2, It2 end2)
                : iterator_base(true), m_begin1(begin1), m_end1(end1), m_begin2(begin2), m_end2(end2) {}
            concat_iterator(const concat_iterator&) = default;
            concat_iterator& operator=(const concat_iterator&) = default;

            void swap(concat_iterator& it)
            {
                iterator_base::swap(it);
                std::swap(m_begin1, it.m_begin1);
                std::swap(m_end1, it.m_end1);
                std::swap(m_begin2, it.m_begin2);
                std::swap(m_end2, it.m_end2);
            }

            reference operator*() const noexcept
            {
                if (m_begin1 != m_end1)
                    return *m_begin1;
                else
                    return *m_begin2;
            }
            pointer operator->() const noexcept { return &operator*(); }

            constexpr bool operator==(const concat_iterator& it) const noexcept
            {
                if (this->m_valid && it.m_valid)
                {
                    return m_begin1 == it.m_begin1 && m_end1 == it.m_end1 && m_begin2 == it.m_begin2 && m_end2 == it.m_end2;
                }
                else
                {
                    return !this->m_valid && !it.m_valid;
                }
            }
            constexpr bool operator!=(const concat_iterator& it) const noexcept { return !(*this == it); }

            constexpr concat_iterator& operator++()
            {
                if (m_begin1 != m_end1)
                    ++m_begin1;
                else
                {
                    ++m_begin2;
                    if (m_begin2 == m_end2) this->m_valid = false;
                }
                return *this;
            }
            concat_iterator operator++(int)
            {
                concat_iterator it = *this;
                operator++();
                return it;
            }
        };
    } // namespace impl

    // Concatenates two enumerable.
    template <typename Container2>
    constexpr auto concat(Container2&& container2)
    {
        return [container2 = std::forward<Container2>(container2)](auto&& container) {
            using It1 = decltype(std::begin(container));
            using It2 = decltype(std::begin(container2));
            return impl::iterable<impl::concat_iterator<It1, It2>>{ impl::concat_iterator<It1, It2>{ std::begin(container), std::end(container), std::begin(container2), std::end(container2) } };
        };
    }
} // namespace linq

#endif // !LINQ_CORE_HPP
