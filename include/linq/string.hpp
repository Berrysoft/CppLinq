#ifndef LINQ_STRING_HPP
#define LINQ_STRING_HPP

#include <linq/core.hpp>
#include <string_view>

namespace linq
{
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
} // namespace linq

#endif // !LINQ_STRING_HPP
