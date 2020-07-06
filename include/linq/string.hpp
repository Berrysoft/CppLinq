/**CppLinq string.hpp
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
#ifndef LINQ_STRING_HPP
#define LINQ_STRING_HPP

#include <linq/core.hpp>
#include <sstream>
#include <string>
#include <string_view>

namespace linq
{
    // SFINAE for character type
    template <typename Char>
    inline constexpr bool is_char_v{ false };

    template <>
    inline constexpr bool is_char_v<char>{ true };

    template <>
    inline constexpr bool is_char_v<wchar_t>{ true };

    template <>
    inline constexpr bool is_char_v<char16_t>{ true };

    template <>
    inline constexpr bool is_char_v<char32_t>{ true };

    namespace impl
    {
        template <typename Char, typename Traits>
        class split_iterator_impl
        {
        private:
            std::basic_string_view<Char, Traits> m_view;
            Char m_char;
            typename Traits::off_type m_offset{ 0 };
            std::size_t m_index;

            using result_type = std::basic_string_view<Char, Traits>;
            result_type m_result{};

        public:
            using traits_type = iterator_impl_traits<result_type>;

            split_iterator_impl(std::basic_string_view<Char, Traits> view, Char split_char)
                : m_view(view), m_char(split_char), m_index(std::basic_string_view<Char, Traits>::npos)
            {
                move_next();
            }

            typename traits_type::reference value() { return m_result; }

            void move_next()
            {
                m_offset = m_index + 1;
                if ((std::size_t)m_offset < m_view.length())
                {
                    m_index = m_view.find(m_char, (std::size_t)m_offset);
                    if (m_index == std::basic_string_view<Char, Traits>::npos) m_index = m_view.length();
                    m_result = m_view.substr((std::size_t)m_offset, (std::size_t)(m_index - m_offset));
                }
            }

            bool is_valid() const { return (std::size_t)m_offset < m_view.length(); }
        };

        template <typename Char, typename Traits>
        using split_iterator = iterator_base<split_iterator_impl<Char, Traits>>;
    } // namespace impl

    // Split the string into an enumerable of string_view by a char.
    template <typename Char, typename Traits = std::char_traits<Char>>
    constexpr auto split(Char split_char = (Char)' ')
    {
        return [=](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            return impl::iterable{ impl::split_iterator<Char, Traits>{ impl::iterator_ctor, view, split_char } };
        };
    }

    // Contacts the string elements into a new string.
    // Named join to distinguish from concat.
    template <typename Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
    constexpr auto joinstr()
    {
        return [](auto&& container) {
            std::basic_ostringstream<Char, Traits, Allocator> oss;
            for (auto& item : container)
            {
                oss << item;
            }
            return oss.str();
        };
    }

    // Inserts something between every two string elements when contacting.
    template <typename Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>, typename T>
    constexpr auto joinstr(T&& value)
    {
        return [&](auto&& container) {
            std::basic_ostringstream<Char, Traits, Allocator> oss;
            auto begin = std::begin(container);
            auto end = std::end(container);
            if (begin != end)
            {
                oss << *begin;
                for (++begin; begin != end; ++begin)
                {
                    oss << value << *begin;
                }
            }
            return oss.str();
        };
    }

    // Determines whether a char or a string span is in the string.
    // Named instr to distinguish from contains, although the latter doesn't exist now.
    template <typename Char, typename Traits = std::char_traits<Char>, typename T>
    constexpr auto instr(T&& t)
    {
        return [&](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            return view.find(std::forward<T>(t)) != std::basic_string_view<Char, Traits>::npos;
        };
    }

    // Determines whether a char is in the start of the string.
    template <typename Char, typename Traits = std::char_traits<Char>, typename = std::enable_if_t<is_char_v<Char>>>
    constexpr auto starts_with(Char value)
    {
        return [=](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            return view.front() == value;
        };
    }

    // Determines whether a string span is in the start of the string.
    template <typename Char, typename Traits = std::char_traits<Char>, typename T>
    constexpr auto starts_with(T&& t)
    {
        return [&](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            std::basic_string_view<Char, Traits> value{ std::forward<T>(t) };
            if (view.length() < value.length())
                return false;
            return view.compare(0, value.length(), value) == 0;
        };
    }

    // Determines whether a char is in the end of the string.
    template <typename Char, typename Traits = std::char_traits<Char>, typename = std::enable_if_t<is_char_v<Char>>>
    constexpr auto ends_with(Char value)
    {
        return [=](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            return view.back() == value;
        };
    }

    // Determines whether a string span is in the end of the string.
    template <typename Char, typename Traits = std::char_traits<Char>, typename T>
    constexpr auto ends_with(T&& t)
    {
        return [&](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            std::basic_string_view<Char, Traits> value{ std::forward<T>(t) };
            if (view.length() < value.length())
                return false;
            return view.compare(view.length() - value.length(), value.length(), value) == 0;
        };
    }

    // Returns a new string with no specified char.
    template <typename Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
    constexpr auto remove(Char value)
    {
        return [=](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            std::basic_ostringstream<Char, Traits, Allocator> oss;
            std::size_t offset{ 0 };
            for (std::size_t i{ 0 }; i < view.length(); i++)
            {
                if (Traits::eq(view[i], value))
                {
                    oss << view.substr(offset, i - offset);
                    offset = i + 1;
                }
            }
            if (offset < view.length())
            {
                oss << view.substr(offset);
            }
            return oss.str();
        };
    }

    // Returns a new string with no specified string span.
    template <typename Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>, typename T>
    constexpr auto remove(T&& t)
    {
        return [&](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            std::basic_string_view<Char, Traits> value{ std::forward<T>(t) };
            std::basic_ostringstream<Char, Traits, Allocator> oss;
            std::size_t offset{ 0 };
            for (std::size_t i{ 0 }; i <= view.length() - value.length(); i++)
            {
                if (view.compare(i, value.length(), value) == 0)
                {
                    oss << view.substr(offset, i - offset);
                    offset = i + value.length();
                    i = offset - 1;
                }
            }
            if (offset < view.length())
            {
                oss << view.substr(offset);
            }
            return oss.str();
        };
    }

    template <typename Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>, typename TNew>
    constexpr auto replace(Char oldc, TNew&& news)
    {
        return [oldc, &news](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            std::basic_ostringstream<Char, Traits, Allocator> oss;
            std::size_t offset{ 0 };
            for (std::size_t i{ 0 }; i < view.length(); i++)
            {
                if (Traits::eq(view[i], oldc))
                {
                    oss << view.substr(offset, i - offset) << news;
                    offset = i + 1;
                }
            }
            if (offset < view.length())
            {
                oss << view.substr(offset);
            }
            return oss.str();
        };
    }

    // Returns a new string which the specified string span is replaced by the new one.
    template <typename Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>, typename TOld, typename TNew>
    constexpr auto replace(TOld&& olds, TNew&& news)
    {
        return [&](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            std::basic_string_view<Char, Traits> value{ std::forward<TOld>(olds) };
            std::basic_ostringstream<Char, Traits, Allocator> oss;
            std::size_t offset{ 0 };
            for (std::size_t i{ 0 }; i <= view.length() - value.length(); i++)
            {
                if (view.compare(i, value.length(), value) == 0)
                {
                    oss << view.substr(offset, i - offset) << news;
                    offset = i + value.length();
                    i = offset - 1;
                }
            }
            if (offset < view.length())
            {
                oss << view.substr(offset);
            }
            return oss.str();
        };
    }

    template <typename Char, typename Traits = std::char_traits<Char>>
    constexpr auto trim(Char value = (Char)' ')
    {
        return [=](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            auto begin{ view.find_first_not_of(value) };
            auto end{ view.find_last_not_of(value) };
            if (begin == std::basic_string_view<Char, Traits>::npos)
                return std::basic_string_view<Char, Traits>{};
            else
                return view.substr(begin, end - begin + 1);
        };
    }

    template <typename Char, typename Traits = std::char_traits<Char>>
    constexpr auto trim_left(Char value = (Char)' ')
    {
        return [=](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            auto begin{ view.find_first_not_of(value) };
            if (begin == std::basic_string_view<Char, Traits>::npos)
                return std::basic_string_view<Char, Traits>{};
            else
                return view.substr(begin);
        };
    }

    template <typename Char, typename Traits = std::char_traits<Char>>
    constexpr auto trim_right(Char value = (Char)' ')
    {
        return [=](auto&& container) {
            std::basic_string_view<Char, Traits> view{ container };
            auto end{ view.find_last_not_of(value) };
            if (end == std::basic_string_view<Char, Traits>::npos)
                return std::basic_string_view<Char, Traits>{};
            else
                return view.substr(0, end + 1);
        };
    }

    namespace impl
    {
        template <typename Char, typename Traits, typename Allocator>
        class read_lines_iterator_impl
        {
        private:
            std::basic_istream<Char, Traits>& m_stream;
            std::optional<std::basic_string<Char, Traits, Allocator>> m_str;

        public:
            using traits_type = iterator_impl_traits<std::basic_string<Char, Traits, Allocator>>;

            read_lines_iterator_impl(std::basic_istream<Char, Traits>& stream) : m_stream(stream)
            {
                move_next();
            }

            typename traits_type::reference value() { return *m_str; }

            void move_next()
            {
                std::basic_string<Char, Traits, Allocator> str;
                if (std::getline(m_stream, str))
                    m_str.emplace(str);
                else
                    m_str = std::nullopt;
            }

            bool is_valid() const { return (bool)m_str; }
        };

        template <typename Char, typename Traits, typename Allocator>
        using read_lines_iterator = iterator_base<read_lines_iterator_impl<Char, Traits, Allocator>>;
    } // namespace impl

    // Read lines of string from a stream.
    template <typename Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
    constexpr auto read_lines(std::basic_istream<Char, Traits>& stream)
    {
        return impl::iterable{ impl::read_lines_iterator<Char, Traits, Allocator>{ impl::iterator_ctor, stream } };
    }

    // Write lines of string to a stream.
    template <typename Char, typename Traits = std::char_traits<Char>, typename C>
    constexpr decltype(auto) write_lines(std::basic_ostream<Char, Traits>& stream, C&& c)
    {
        for (auto& item : c)
        {
            stream << item << stream.widen('\n');
        }
        return stream;
    }
} // namespace linq

#endif // !LINQ_STRING_HPP
