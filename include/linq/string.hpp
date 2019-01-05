#ifndef LINQ_STRING_HPP
#define LINQ_STRING_HPP

#include <linq/core.hpp>
#include <sstream>
#include <string_view>

namespace linq
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

    template <>
    struct is_char<char16_t> : std::true_type
    {
    };

    template <>
    struct is_char<char32_t> : std::true_type
    {
    };

    template <typename Char>
    constexpr bool is_char_v = is_char<Char>::value;

    // split enumerator
    namespace impl
    {
        template <typename Char, typename Traits>
        class split_enumerator
        {
        private:
            std::basic_string_view<Char, Traits> m_view;
            Char m_char;
            typename Traits::off_type m_offset, m_index;

            void move_next()
            {
                m_offset = m_index + 1;
                if (m_offset < m_view.length())
                {
                    m_index = m_view.find(m_char, m_offset);
                    if (m_index == std::basic_string_view<Char, Traits>::npos)
                        m_index = m_view.length();
                }
            }

        public:
            constexpr split_enumerator(std::basic_string_view<Char, Traits> view, Char split_char) : m_view(view), m_char(split_char), m_offset(0), m_index(-1)
            {
                move_next();
            }

            constexpr operator bool() const { return m_offset < m_view.length(); }
            constexpr split_enumerator& operator++()
            {
                move_next();
                return *this;
            }
            constexpr auto operator*() { return m_view.substr(m_offset, m_index - m_offset); }
        };
    } // namespace impl

    template <typename Char, typename Traits = std::char_traits<Char>>
    constexpr auto split(Char split_char)
    {
        return [=](auto e) {
            using Container = decltype(e.enumerator().container());
            static_assert(std::is_convertible_v<Container, std::basic_string_view<Char, Traits>>, "The enumerable should be an adapter of a string_view.");
            std::basic_string_view<Char, Traits> view{ e.enumerator().container() };
            return enumerable(impl::split_enumerator(view, split_char));
        };
    }

    // Contacts the string elements into a new string.
    // Named join to distinguish from concat.
    template <typename Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
    constexpr auto join()
    {
        return [](auto e) {
            auto eter{ e.enumerator() };
            std::basic_ostringstream<Char, Traits, Allocator> oss;
            for (; eter; ++eter)
            {
                oss << *eter;
            }
            return oss.str();
        };
    }

    // Inserts something between every two string elements when contacting.
    template <typename Char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>, typename T>
    constexpr auto join(T&& value)
    {
        return [&](auto e) {
            auto eter{ e.enumerator() };
            std::basic_ostringstream<Char, Traits, Allocator> oss;
            if (eter)
            {
                oss << *eter;
                for (++eter; eter; ++eter)
                {
                    oss << value << *eter;
                }
            }
            return oss.str();
        };
    }

    // Determines whether a char is in the string.
    // Named instr to distinguish from contains, although the latter doesn't exist now.
    template <typename Char, typename Traits = std::char_traits<Char>, typename = std::enable_if_t<is_char_v<Char>>>
    constexpr auto instr(Char value)
    {
        return [=](auto e) {
            using Container = decltype(e.enumerator().container());
            static_assert(std::is_convertible_v<Container, std::basic_string_view<Char, Traits>>, "The enumerable should be an adapter of a string_view.");
            std::basic_string_view<Char, Traits> view{ e.enumerator().container() };
            return view.find(value) != std::basic_string_view<Char, Traits>::npos;
        };
    }

    // Determines whether a string span is in the string.
    template <typename Char, typename Traits = std::char_traits<Char>, typename T>
    constexpr auto instr(T&& t)
    {
        return [&](auto e) {
            using Container = decltype(e.enumerator().container());
            static_assert(std::is_convertible_v<Container, std::basic_string_view<Char, Traits>>, "The enumerable should be an adapter of a string_view.");
            std::basic_string_view<Char, Traits> view{ e.enumerator().container() };
            std::basic_string_view<Char, Traits> value{ std::forward<T>(t) };
            for (std::size_t i{ 0 }; i < view.length(); i++)
            {
                for (std::size_t j{ 0 }; j < value.length(); j++)
                {
                    if (!Traits::eq(view[i + j], value[j]))
                        goto continue_outer;
                }
                return true;
            continue_outer:;
            }
            return false;
        };
    }
} // namespace linq

#endif // !LINQ_STRING_HPP
