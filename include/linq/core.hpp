/**CppLinq core.hpp
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
#ifndef LINQ_CORE_HPP
#define LINQ_CORE_HPP

#include <iterator>
#include <memory>
#include <optional>

namespace linq
{
    namespace impl
    {
        template <typename T, typename Ref = const T&, typename Ptr = const T*>
        struct iterator_impl_traits
        {
            using value_type = T;
            using reference = Ref;
            using pointer = Ptr;
        };

        struct iterator_ctor_t
        {
        };

        inline constexpr iterator_ctor_t iterator_ctor{};

        template <typename Impl>
        class iterator_base
        {
        private:
            std::shared_ptr<Impl> m_impl{};

        public:
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using __traits_type = typename Impl::traits_type;
            using value_type = typename __traits_type::value_type;
            using pointer = typename __traits_type::pointer;
            using reference = typename __traits_type::reference;

            iterator_base() noexcept = default;
            template <typename... Args>
            iterator_base(iterator_ctor_t, Args&&... args) : m_impl(std::make_shared<Impl>(std::forward<Args>(args)...))
            {
                if (!(m_impl->is_valid())) m_impl = nullptr;
            }
            iterator_base(const iterator_base&) noexcept(std::is_nothrow_copy_constructible_v<std::shared_ptr<Impl>>) = default;
            iterator_base& operator=(const iterator_base&) noexcept(std::is_nothrow_copy_assignable_v<std::shared_ptr<Impl>>) = default;

            reference operator*() const { return m_impl->value(); }
            pointer operator->() const { return std::pointer_traits<pointer>::pointer_to(m_impl->value()); }

            constexpr operator bool() const noexcept { return static_cast<bool>(m_impl); }

            bool operator==(const iterator_base& it) const noexcept
            {
                return this == std::addressof(it) || (!m_impl && !it.m_impl);
            }
            bool operator!=(const iterator_base& it) const noexcept { return !operator==(it); }

            iterator_base& operator++()
            {
                m_impl->move_next();
                if (!(m_impl->is_valid())) m_impl = nullptr;
                return *this;
            }
            iterator_base operator++(int)
            {
                iterator_base it = this;
                operator++();
                return it;
            }
        };

        template <typename It>
        class iterable
        {
        private:
            It m_begin{}, m_end{};

        public:
            iterable(It&& begin) noexcept(std::is_nothrow_move_constructible_v<It>&& std::is_nothrow_constructible_v<It>)
                : m_begin(std::move(begin)), m_end()
            {
            }

            It begin() const noexcept(std::is_nothrow_copy_constructible_v<It>) { return m_begin; }
            It end() const noexcept(std::is_nothrow_copy_constructible_v<It>) { return m_end; }
        };

        template <typename It>
        iterable(It &&) -> iterable<It>;
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
        class range_iterator_impl
        {
        private:
            T m_current{}, m_end{};
            std::decay_t<Func> m_func;

        public:
            using traits_type = iterator_impl_traits<T>;

            range_iterator_impl(T&& begin, T&& end, Func&& func) noexcept(std::is_nothrow_move_constructible_v<T>&& std::is_nothrow_move_constructible_v<Func>)
                : m_current(std::move(begin)), m_end(std::move(end)), m_func(std::forward<Func>(func)) {}

            typename traits_type::reference value() const noexcept { return m_current; }

            void move_next() { m_current = m_func(m_current); }

            bool is_valid() const noexcept { return m_current < m_end; }
        };

        template <typename T, typename Func>
        using range_iterator = iterator_base<range_iterator_impl<T, Func>>;
    } // namespace impl

    template <typename T, typename Func = impl::increase<T>>
    constexpr auto range(T begin, T end, Func&& func = {})
    {
        return impl::iterable{ impl::range_iterator<T, Func>{ impl::iterator_ctor, std::move(begin), std::move(end), std::forward<Func>(func) } };
    }

    template <typename T>
    constexpr auto range(T begin, T end, T step)
    {
        return range<T>(std::move(begin), std::move(end), [step = std::move(step)](const T& value) { return value + step; });
    }

    namespace impl
    {
        template <typename T>
        class repeat_iterator_impl
        {
        private:
            T m_value;
            std::size_t m_count{ 0 };

        public:
            using traits_type = iterator_impl_traits<T>;

            constexpr repeat_iterator_impl(T&& value, std::size_t count) noexcept(std::is_nothrow_move_constructible_v<T>)
                : m_value(std::move(value)), m_count(count) {}

            typename traits_type::reference value() const noexcept { return m_value; }

            void move_next() { --m_count; }

            bool is_valid() const noexcept { return m_count > 0; }
        };

        template <typename T>
        using repeat_iterator = iterator_base<repeat_iterator_impl<T>>;
    } // namespace impl

    template <typename T>
    constexpr auto repeat(T value, std::size_t count)
    {
        return impl::iterable{ impl::repeat_iterator<T>{ impl::iterator_ctor, std::move(value), count } };
    }

    namespace impl
    {
        template <typename T, typename It>
        class append_iterator_impl
        {
        private:
            std::optional<T> m_value;
            It m_begin, m_end;

        public:
            using traits_type = iterator_impl_traits<std::common_type_t<T, typename std::iterator_traits<It>::value_type>>;

            append_iterator_impl(T value, It begin, It end) : m_value(std::move(value)), m_begin(begin), m_end(end) {}

            typename traits_type::reference value() const noexcept
            {
                if (m_begin != m_end)
                    return *m_begin;
                else
                    return *m_value;
            }

            void move_next()
            {
                if (m_begin != m_end)
                    ++m_begin;
                else
                    m_value = std::nullopt;
            }

            bool is_valid() const { return m_begin != m_end || m_value; }
        };

        template <typename T, typename It>
        using append_iterator = iterator_base<append_iterator_impl<T, It>>;
    } // namespace impl

    // Appends an element to the enumerable.
    template <typename T>
    constexpr auto append(T&& value)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::append_iterator<std::remove_reference_t<T>, It>{ impl::iterator_ctor, std::forward<T>(value), std::begin(container), std::end(container) } };
        };
    }

    namespace impl
    {
        template <typename T, typename It>
        class prepend_iterator_impl
        {
        private:
            std::optional<T> m_value;
            It m_begin, m_end;

        public:
            using traits_type = iterator_impl_traits<std::common_type_t<T, typename std::iterator_traits<It>::value_type>>;

            prepend_iterator_impl(T value, It begin, It end) : m_value(std::move(value)), m_begin(begin), m_end(end) {}

            typename traits_type::reference value() const noexcept
            {
                if (m_value)
                    return *m_value;
                else
                    return *m_begin;
            }

            void move_next()
            {
                if (m_value)
                    m_value = std::nullopt;
                else
                    ++m_begin;
            }

            bool is_valid() const { return m_value || m_begin != m_end; }
        };

        template <typename T, typename It>
        using prepend_iterator = iterator_base<prepend_iterator_impl<T, It>>;
    } // namespace impl

    // Prepends an element to the enumerable.
    template <typename T>
    constexpr auto prepend(T&& value)
    {
        return [&](auto&& container) {
            using It = decltype(std::begin(container));
            return impl::iterable{ impl::prepend_iterator<std::remove_reference_t<T>, It>{ impl::iterator_ctor, std::forward<T>(value), std::begin(container), std::end(container) } };
        };
    }

    namespace impl
    {
        template <typename It1, typename It2>
        class concat_iterator_impl
        {
        private:
            It1 m_begin1, m_end1;
            It2 m_begin2, m_end2;

        public:
            using traits_type = iterator_impl_traits<std::common_type_t<
                typename std::iterator_traits<It1>::value_type,
                typename std::iterator_traits<It2>::value_type>>;

            concat_iterator_impl(It1 begin1, It1 end1, It2 begin2, It2 end2)
                : m_begin1(begin1), m_end1(end1), m_begin2(begin2), m_end2(end2) {}

            typename traits_type::reference value() const noexcept
            {
                if (m_begin1 != m_end1)
                    return *m_begin1;
                else
                    return *m_begin2;
            }

            void move_next()
            {
                if (m_begin1 != m_end1)
                    ++m_begin1;
                else
                    ++m_begin2;
            }

            bool is_valid() const { return m_begin1 != m_end1 || m_begin2 != m_end2; }
        };

        template <typename It1, typename It2>
        using concat_iterator = iterator_base<concat_iterator_impl<It1, It2>>;
    } // namespace impl

    // Concatenates two enumerable.
    template <typename Container2>
    constexpr auto concat(Container2&& container2)
    {
        return [&](auto&& container) {
            using It1 = decltype(std::begin(container));
            using It2 = decltype(std::begin(container2));
            return impl::iterable{ impl::concat_iterator<It1, It2>{
                impl::iterator_ctor, std::begin(container), std::end(container), std::begin(container2), std::end(container2) } };
        };
    }
} // namespace linq

#endif // !LINQ_CORE_HPP
