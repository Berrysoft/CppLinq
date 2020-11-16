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
    namespace impl
    {
        template <container Container, typename Char, typename Traits>
        std::basic_string_view<Char, Traits> get_string_view(Container&& container)
        {
            if constexpr (std::is_convertible_v<Container&&, std::basic_string_view<Char, Traits>>)
            {
                return static_cast<std::basic_string_view<Char, Traits>>(std::forward<Container>(container));
            }
            else
            {
                return std::basic_string_view<Char, Traits>(std::begin(container), std::end(container));
            }
        }
    } // namespace impl

    // Split the string into an enumerable of string_view by a char.
    template <impl::character Char, typename Traits = std::char_traits<Char>>
    struct split
    {
        Char m_char;

        constexpr split(Char split_char = (Char)' ') noexcept : m_char(split_char) {}

        template <impl::container Container>
        auto operator()(Container container) const
            -> generator<std::basic_string_view<Char, Traits>>
        {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::move(container));
            if (!view.empty())
            {
                typename Traits::off_type offset{ 0 };
                std::size_t index{ std::basic_string<Char, Traits>::npos };
                while (true)
                {
                    offset = index + 1;
                    if ((std::size_t)offset < view.length())
                    {
                        index = view.find(m_char, (std::size_t)offset);
                        if (index == std::basic_string_view<Char, Traits>::npos) index = view.length();
                        co_yield view.substr((std::size_t)offset, (std::size_t)(index - offset));
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    };

    template <typename Char>
    split(Char) -> split<Char>;

    // Contacts the string elements into a new string.
    // Named join to distinguish from concat.
    template <impl::character Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
    constexpr auto joinstr()
    {
        return []<impl::container Container>(Container container) {
            std::basic_ostringstream<Char, Traits, Allocator> oss;
            for (auto&& item : container)
            {
                oss << item;
            }
            return oss.str();
        };
    }

    // Inserts something between every two string elements when contacting.
    template <impl::character Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>, typename T>
    constexpr auto joinstr(T&& value)
    {
        return [=]<impl::container Container>(Container container) {
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

    // Determines whether a char is in the string.
    // Named instr to distinguish from contains, although the latter doesn't exist now.
    template <impl::character Char, typename Traits = std::char_traits<Char>>
    constexpr auto instr(Char t)
    {
        return [=]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
            return view.find(t) != std::basic_string_view<Char, Traits>::npos;
        };
    }

    // Determines whether a string span is in the string.
    // Named instr to distinguish from contains, although the latter doesn't exist now.
    template <impl::character Char, typename Traits = std::char_traits<Char>, typename T>
    constexpr auto instr(T&& t) requires(!impl::is_char_v<std::remove_cvref_t<T>>)
    {
        return [&]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
            std::basic_string_view<Char, Traits> tview = impl::get_string_view<T, Char, Traits>(std::forward<T>(t));
            return view.find(tview) != std::basic_string_view<Char, Traits>::npos;
        };
    }

    // Determines whether a char is in the start of the string.
    template <impl::character Char, typename Traits = std::char_traits<Char>>
    constexpr auto starts_with(Char value)
    {
        return [=]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
            return view.front() == value;
        };
    }

    // Determines whether a string span is in the start of the string.
    template <impl::character Char, typename Traits = std::char_traits<Char>, typename T>
    constexpr auto starts_with(T&& t) requires(!impl::is_char_v<std::remove_cvref_t<T>>)
    {
        return [&]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
            std::basic_string_view<Char, Traits> value = impl::get_string_view<T, Char, Traits>(std::forward<T>(t));
            if (view.length() < value.length())
                return false;
            return view.compare(0, value.length(), value) == 0;
        };
    }

    // Determines whether a char is in the end of the string.
    template <impl::character Char, typename Traits = std::char_traits<Char>>
    constexpr auto ends_with(Char value)
    {
        return [=]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
            return view.back() == value;
        };
    }

    // Determines whether a string span is in the end of the string.
    template <impl::character Char, typename Traits = std::char_traits<Char>, typename T>
    constexpr auto ends_with(T&& t) requires(!impl::is_char_v<std::remove_cvref_t<T>>)
    {
        return [&]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
            std::basic_string_view<Char, Traits> value = impl::get_string_view<T, Char, Traits>(std::forward<T>(t));
            if (view.length() < value.length())
                return false;
            return view.compare(view.length() - value.length(), value.length(), value) == 0;
        };
    }

    // Returns a new string with no specified char.
    template <impl::character Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
    constexpr auto remove(Char value)
    {
        return [=]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
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
    template <impl::character Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>, typename T>
    constexpr auto remove(T&& t) requires(!impl::is_char_v<std::remove_cvref_t<T>>)
    {
        return [&]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
            std::basic_string_view<Char, Traits> value = impl::get_string_view<T, Char, Traits>(std::forward<T>(t));
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

    template <impl::character Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>, typename TNew>
    constexpr auto replace(Char oldc, TNew&& news)
    {
        return [=]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
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
    template <impl::character Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>, typename TOld, typename TNew>
    constexpr auto replace(TOld&& olds, TNew&& news) requires(!impl::is_char_v<std::remove_cvref_t<TOld>>)
    {
        return [&]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
            std::basic_string_view<Char, Traits> value = impl::get_string_view<TOld, Char, Traits>(std::forward<TOld>(olds));
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

    template <impl::character Char, typename Traits = std::char_traits<Char>>
    constexpr auto trim(Char value = (Char)' ')
    {
        return [=]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
            auto begin{ view.find_first_not_of(value) };
            auto end{ view.find_last_not_of(value) };
            if (begin == std::basic_string_view<Char, Traits>::npos)
                return std::basic_string_view<Char, Traits>{};
            else
                return view.substr(begin, end - begin + 1);
        };
    }

    template <impl::character Char, typename Traits = std::char_traits<Char>>
    constexpr auto trim_left(Char value = (Char)' ')
    {
        return [=]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
            auto begin{ view.find_first_not_of(value) };
            if (begin == std::basic_string_view<Char, Traits>::npos)
                return std::basic_string_view<Char, Traits>{};
            else
                return view.substr(begin);
        };
    }

    template <impl::character Char, typename Traits = std::char_traits<Char>>
    constexpr auto trim_right(Char value = (Char)' ')
    {
        return [=]<impl::container Container>(Container&& container) {
            std::basic_string_view<Char, Traits> view = impl::get_string_view<Container, Char, Traits>(std::forward<Container>(container));
            auto end{ view.find_last_not_of(value) };
            if (end == std::basic_string_view<Char, Traits>::npos)
                return std::basic_string_view<Char, Traits>{};
            else
                return view.substr(0, end + 1);
        };
    }

    template <impl::character Char>
    struct read_lines
    {
        Char m_delim;

        constexpr read_lines(Char delim = (Char)'\n') noexcept : m_delim(delim) {}
    };

    template <typename Char>
    read_lines(Char) -> read_lines<Char>;

    // Read lines of string from a stream.
    template <impl::character Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
    generator<std::basic_string<Char, Traits, Allocator>> operator>>(std::basic_istream<Char, Traits>& stream, read_lines<Char> r)
    {
        std::basic_string<Char, Traits, Allocator> str;
        while (std::getline(stream, str, r.m_delim))
        {
            co_yield str;
        }
    }

    // Write lines of string to a stream.
    template <impl::character Char, typename Traits = std::char_traits<Char>, impl::container C>
    decltype(auto) operator<<(std::basic_ostream<Char, Traits>& stream, C&& c) requires requires(typename impl::container_traits<C>::value_type item)
    {
        {
            stream << item
        }
        ->std::convertible_to<decltype(stream)>;
    }
    {
        for (auto&& item : c)
        {
            stream << item << stream.widen('\n');
        }
        return stream;
    }
} // namespace linq

#endif // !LINQ_STRING_HPP
