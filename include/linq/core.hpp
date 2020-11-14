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

#include <coroutine>
#include <iterator>
#include <ranges>
#include <string_view>

namespace linq
{
    template <typename T>
    struct generator
    {
        struct promise_type
        {
            T current_value;

            auto get_return_object() { return generator{ *this }; }

            std::suspend_always initial_suspend() noexcept { return {}; }

            std::suspend_always final_suspend() noexcept { return {}; }

            void unhandled_exception() { throw; }

            std::suspend_always yield_value(T const& value)
            {
                if constexpr (std::is_move_assignable_v<T>)
                {
                    current_value = std::move(value);
                }
                else
                {
                    current_value = value;
                }
                return {};
            }

            void return_void() {}

            template <typename _Uty>
            _Uty&& await_transform(_Uty&& _Whatever) = delete;
        };

        struct iterator
        {
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using reference = T const&;
            using pointer = T const*;

            std::coroutine_handle<promise_type> coro = nullptr;

            iterator() = default;
            iterator(std::nullptr_t) : coro(nullptr) {}

            iterator(std::coroutine_handle<promise_type> coro) : coro(coro) {}

            iterator& operator++()
            {
                coro.resume();
                if (coro.done())
                {
                    coro = nullptr;
                }
                return *this;
            }

            void operator++(int) { ++*this; }

            [[nodiscard]] operator bool() const { return coro && !coro.done(); }

            [[nodiscard]] bool operator==(iterator const& right) const
            {
                return !*this && !right;
            }

            [[nodiscard]] reference operator*() const
            {
                return coro.promise().current_value;
            }

            [[nodiscard]] pointer operator->() const
            {
                return &coro.promise().current_value;
            }
        };

        [[nodiscard]] iterator begin() const
        {
            if (coro)
            {
                coro.resume();
                if (coro.done())
                {
                    return { nullptr };
                }
            }
            return { coro };
        }

        [[nodiscard]] iterator end() const
        {
            return { nullptr };
        }

        explicit generator(promise_type& prom) : coro(std::coroutine_handle<promise_type>::from_promise(prom)) {}

        generator() = default;
        generator(generator const&) = delete;
        generator& operator=(generator const&) = delete;

        generator(generator&& right) : coro(right.coro)
        {
            right.coro = nullptr;
        }

        generator& operator=(generator&& right)
        {
            if (this != std::addressof(right))
            {
                coro = right.coro;
                right.coro = nullptr;
            }
            return *this;
        }

        ~generator()
        {
            if (coro)
            {
                coro.destroy();
            }
        }

    private:
        std::coroutine_handle<promise_type> coro = nullptr;
    };

    // SFINAE for character type
    template <typename Char>
    inline constexpr bool is_char_v{ false };

    template <>
    inline constexpr bool is_char_v<char>{ true };

    template <>
    inline constexpr bool is_char_v<wchar_t>{ true };

#ifdef __cpp_char8_t
    template <>
    inline constexpr bool is_char_v<char8_t>{ true };
#endif // __cpp_char8_t

    template <>
    inline constexpr bool is_char_v<char16_t>{ true };

    template <>
    inline constexpr bool is_char_v<char32_t>{ true };

    namespace impl
    {
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
        concept reversible_container = container<T>&& requires(T&& t)
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
        struct is_wrapped_reversible_container : std::false_type
        {
        };

        template <reversible_container T>
        struct is_wrapped_reversible_container<std::ranges::ref_view<T>> : std::true_type
        {
        };

        template <typename T>
        inline constexpr bool is_wrapped_reversible_container_v = is_wrapped_reversible_container<T>::value;

        template <container T>
        decltype(auto) decay_container(T&& c)
        {
            if constexpr (std::is_lvalue_reference_v<T&&>)
            {
                if constexpr (is_char_v<typename std::iterator_traits<decltype(std::begin(c))>::value_type>)
                {
                    return std::string_view{ c };
                }
                else
                {
                    return std::ranges::ref_view{ c };
                }
            }
            else
            {
                return std::forward<T>(c);
            }
        }
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
        return range<T>(std::move(begin), std::move(end), [step = std::move(step)](const T& value) { return value + step; });
    }

    template <typename T>
    generator<T> repeat(T value, std::size_t count)
    {
        while (count--)
        {
            co_yield value;
        }
    }

    // Appends an element to the enumerable.
    template <typename T>
    constexpr auto append(T&& value)
    {
        return [=]<impl::container Container>(Container container)
                   -> generator<std::remove_cvref_t<std::common_type_t<
                       typename std::iterator_traits<decltype(std::begin(container))>::value_type, T>>> {
            for (auto&& item : container)
            {
                co_yield item;
            }
            co_yield value;
        };
    }

    // Prepends an element to the enumerable.
    template <typename T>
    constexpr auto prepend(T&& value)
    {
        return [=]<impl::container Container>(Container container)
                   -> generator<std::remove_cvref_t<std::common_type_t<
                       typename std::iterator_traits<decltype(std::begin(container))>::value_type, T>>> {
            co_yield value;
            for (auto&& item : container)
            {
                co_yield item;
            }
        };
    }

    // Concatenates two enumerable.
    template <impl::container Container2>
    constexpr auto concat(Container2&& container2)
    {
        return [container2 = impl::decay_container(std::forward<Container2>(container2))]<impl::container Container>(Container container)
                   -> generator<std::remove_cvref_t<std::common_type_t<
                       typename std::iterator_traits<decltype(std::begin(container))>::value_type,
                       typename std::iterator_traits<decltype(std::begin(container2))>::value_type>>> {
            for (auto&& item : container)
            {
                co_yield item;
            }
            for (auto&& item : container2)
            {
                co_yield item;
            }
        };
    }
} // namespace linq

#endif // !LINQ_CORE_HPP
