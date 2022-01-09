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

#include <exception>
#include <iterator>
#include <span>
#include <string_view>
#include <tuple>
#include <version>

#if __cpp_lib_coroutine >= 201902L || __has_include(<coroutine>)
    #include <coroutine>
#else
    #define LINQ_USE_EXPERIMENTAL_COROUTINE
    #include <experimental/coroutine>
#endif

namespace linq
{
#ifdef LINQ_USE_EXPERIMENTAL_COROUTINE
    using std::experimental::coroutine_handle;
    using std::experimental::suspend_always;
#else
    using std::coroutine_handle;
    using std::suspend_always;
#endif // LINQ_USE_EXPERIMENTAL_COROUTINE

    template <typename T>
    requires(std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>) struct generator
    {
        struct promise_type
        {
            T m_current;
            std::exception_ptr m_exception;

            auto get_return_object() { return generator{ *this }; }

            suspend_always initial_suspend() const noexcept { return {}; }
            suspend_always final_suspend() const noexcept { return {}; }

            void unhandled_exception() noexcept { m_exception = std::current_exception(); }

            void rethrow_if_exception() const
            {
                if (m_exception)
                {
                    std::rethrow_exception(m_exception);
                }
            }

            suspend_always yield_value(T const& value)
            {
                if constexpr (std::is_move_assignable_v<T>)
                {
                    m_current = std::move(value);
                }
                else
                {
                    m_current = value;
                }
                return {};
            }

            void return_void() const noexcept {}

            template <typename U>
            U&& await_transform(U&&) const noexcept = delete;
        };

        struct iterator
        {
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using reference = T const&;
            using pointer = T const*;

            coroutine_handle<promise_type> m_coro = nullptr;

            iterator() = default;
            iterator(std::nullptr_t) : m_coro(nullptr) {}

            iterator(coroutine_handle<promise_type> coro) : m_coro(coro) {}

            iterator& operator++()
            {
                m_coro.resume();
                if (m_coro.done())
                {
                    m_coro.promise().rethrow_if_exception();
                    m_coro = nullptr;
                }
                return *this;
            }

            void operator++(int) { ++*this; }

            [[nodiscard]] operator bool() const { return m_coro && !m_coro.done(); }

            [[nodiscard]] bool operator==(iterator const& right) const noexcept
            {
                return m_coro == right.m_coro;
            }

            [[nodiscard]] reference operator*() const
            {
                return m_coro.promise().m_current;
            }

            [[nodiscard]] pointer operator->() const
            {
                return &m_coro.promise().m_current;
            }
        };

        [[nodiscard]] iterator begin()
        {
            if (m_coro)
            {
                m_coro.resume();
                if (m_coro.done())
                {
                    m_coro.promise().rethrow_if_exception();
                    return { nullptr };
                }
            }
            return { m_coro };
        }

        [[nodiscard]] iterator end()
        {
            return { nullptr };
        }

        explicit generator(promise_type& prom) : m_coro(coroutine_handle<promise_type>::from_promise(prom)) {}

        generator() = default;
        generator(generator const&) = delete;
        generator& operator=(generator const&) = delete;

        generator(generator&& right) noexcept : m_coro(std::exchange(right.m_coro, nullptr)) {}

        generator& operator=(generator&& right) noexcept
        {
            if (this != std::addressof(right))
            {
                swap(right);
            }
            return *this;
        }

        void swap(generator& other) noexcept { std::swap(m_coro, other.m_coro); }

        ~generator()
        {
            if (m_coro)
            {
                m_coro.destroy();
            }
        }

    private:
        coroutine_handle<promise_type> m_coro = nullptr;
    };

    namespace impl
    {

        // SFINAE for character type
        template <typename Char>
        struct is_char : std::false_type
        {
        };

        template <>
        struct is_char<char> : std::true_type
        {
        };

        template <>
        struct is_char<wchar_t> : std::true_type
        {
        };

#ifdef __cpp_char8_t
        template <>
        struct is_char<char8_t> : std::true_type
        {
        };
#endif // __cpp_char8_t

        template <>
        struct is_char<char16_t> : std::true_type
        {
        };

        template <>
        struct is_char<char32_t> : std::true_type
        {
        };

        template <typename Char>
        inline constexpr bool is_char_v = is_char<Char>::value;

        template <typename Char>
        concept character = is_char_v<Char>;

        template <typename T>
        concept container = requires(T&& t)
        {
            std::begin(t);
            std::end(t);
        };

        template <typename T>
        struct is_container : std::false_type
        {
        };

        template <container T>
        struct is_container<T> : std::true_type
        {
        };

        template <typename T>
        inline constexpr bool is_container_v = is_container<T>::value;

        template <typename T>
        concept reversible_container = container<T> && requires(T&& t)
        {
            std::rbegin(t);
            std::rend(t);
        };

        template <typename T>
        struct is_reversible_container : std::false_type
        {
        };

        template <reversible_container T>
        struct is_reversible_container<T> : std::true_type
        {
        };

        template <typename T>
        inline constexpr bool is_reversible_container_v = is_reversible_container<T>::value;

        template <typename T>
        class container_view_base
        {
        private:
            T* m_container_ptr;

        public:
            container_view_base(T& container) : m_container_ptr(std::addressof(container)) {}

            constexpr T& base() const noexcept { return *m_container_ptr; }

            auto begin() const noexcept(noexcept(std::begin(this->base()))) { return std::begin(this->base()); }
            auto end() const noexcept(noexcept(std::end(this->base()))) { return std::end(this->base()); }
        };

        template <typename T>
        class container_view;

        template <container T>
        class container_view<T> : public container_view_base<T>
        {
        public:
            using container_view_base<T>::container_view_base;
        };

        template <reversible_container T>
        class container_view<T> : public container_view_base<T>
        {
        public:
            using container_view_base<T>::container_view_base;

            auto rbegin() const noexcept(noexcept(std::rbegin(this->base()))) { return std::rbegin(this->base()); }
            auto rend() const noexcept(noexcept(std::rend(this->base()))) { return std::rend(this->base()); }
        };

        template <typename T>
        container_view(T&) -> container_view<T>;

        template <typename T>
        struct is_generator : std::false_type
        {
        };

        template <typename T>
        struct is_generator<generator<T>> : std::true_type
        {
        };

        template <typename T>
        inline constexpr bool is_generator_v = is_generator<T>::value;

        template <typename T>
        constexpr T& remove_const(T const& value) noexcept
        {
            return const_cast<T&>(value);
        }

        template <typename T>
        constexpr T&& move_const(T const& value) noexcept
        {
            return std::move(const_cast<T&>(value));
        }

        template <container T>
        auto decay_container(T&& c)
        {
            if constexpr (std::is_lvalue_reference_v<T&&>)
            {
                if constexpr (is_char_v<typename std::iterator_traits<decltype(std::begin(c))>::value_type>)
                {
                    return std::string_view(c);
                }
                else if constexpr (std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<decltype(std::begin(c))>::iterator_category>)
                {
                    return std::span<typename std::iterator_traits<decltype(std::begin(c))>::value_type>(c);
                }
                else
                {
                    return container_view(c);
                }
            }
            else
            {
                return std::forward<T>(c);
            }
        }

        template <typename T>
        using decay_container_t = decltype(decay_container(std::declval<T&&>()));

        template <container T>
        struct container_traits : public std::iterator_traits<decltype(std::begin(std::declval<T&>()))>
        {
            using iterator = decltype(std::begin(std::declval<T&>()));
        };
    } // namespace impl

    template <impl::container Container, typename Query>
    decltype(auto) operator>>(Container&& container, Query&& query)
    {
        return std::forward<Query>(query)(impl::decay_container<Container>(std::forward<Container>(container)));
    }

    namespace impl
    {
        template <typename T>
        struct increase
        {
            constexpr T operator()(T value) noexcept(noexcept(++value)) { return ++value; }
        };
    } // namespace impl

    template <typename T, typename Func = impl::increase<T>>
    generator<T> range(T begin, T end, Func func = {})
    {
        while (begin != end)
        {
            co_yield begin;
            begin = func(begin);
        }
    }

    template <typename T>
    auto range(T begin, T end, T step)
    {
        return range<T>(std::move(begin), std::move(end), [step = std::move(step)](const T& value)
                        { return value + step; });
    }

    template <typename T>
    generator<T> repeat(T value, std::size_t count)
    {
        while (count--)
        {
            co_yield value;
        }
    }

    namespace impl
    {
        template <impl::container Container, typename T>
        auto append(Container container, T value)
            -> generator<std::remove_cvref_t<std::common_type_t<
                typename impl::container_traits<Container>::value_type,
                T>>>
        {
            for (auto&& item : container)
            {
                co_yield item;
            }
            co_yield value;
        }
    } // namespace impl

    // Appends an element to the enumerable.
    template <typename T>
    auto append(T&& value)
    {
        return [=]<impl::container Container>(Container&& container)
        {
            return impl::append<Container, T>(std::forward<Container>(container), std::move(value));
        };
    }

    namespace impl
    {
        template <impl::container Container, typename T>
        auto prepend(Container container, T value)
            -> generator<std::remove_cvref_t<std::common_type_t<
                typename impl::container_traits<Container>::value_type,
                T>>>
        {
            co_yield value;
            for (auto&& item : container)
            {
                co_yield item;
            }
        }
    } // namespace impl

    // Prepends an element to the enumerable.
    template <typename T>
    auto prepend(T&& value)
    {
        return [=]<impl::container Container>(Container&& container)
        {
            return impl::prepend<Container, T>(std::forward<Container>(container), std::move(value));
        };
    }

    namespace impl
    {
        template <impl::container Container, container Container2>
        auto concat(Container container, Container2 container2)
            -> generator<std::remove_cvref_t<std::common_type_t<
                typename impl::container_traits<Container>::value_type,
                typename impl::container_traits<Container2>::value_type>>>
        {
            for (auto&& item : container)
            {
                co_yield item;
            }
            for (auto&& item : container2)
            {
                co_yield item;
            }
        }
    } // namespace impl

    // Concatenates two enumerable.
    template <impl::container Container2>
    auto concat(Container2&& container2)
    {
        return [container2 = impl::decay_container(std::forward<Container2>(container2))]<impl::container Container>(Container&& container)
        {
            return impl::concat<Container, impl::decay_container_t<Container2>>(std::forward<Container>(container), impl::move_const(container2));
        };
    }

    namespace impl
    {
        template <impl::container Container>
        auto with_index(Container container)
            -> generator<std::tuple<std::size_t, typename impl::container_traits<Container>::value_type>>
        {
            std::size_t index{ 0 };
            for (auto&& item : container)
            {
                co_yield std::make_tuple(index, item);
                index++;
            }
        }
    } // namespace impl

    // Zip with index.
    inline auto with_index()
    {
        return []<impl::container Container>(Container&& container)
        {
            return impl::with_index<Container>(std::forward<Container>(container));
        };
    }
} // namespace linq

#endif // !LINQ_CORE_HPP
