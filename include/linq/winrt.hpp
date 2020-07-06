/**CppLinq winrt.hpp
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
#ifndef LINQ_WINRT_HPP
#define LINQ_WINRT_HPP

#include <linq/core.hpp>

#ifdef LINQ_USE_WINRT

namespace linq
{
    namespace impl
    {
        // We have to write our own iterator for WinRT collections
        // because of the wrong typedefs of current implementation

        template <typename It>
        class winrt_iterator_impl
        {
        private:
            It m_begin, m_end;

            using result_type = std::remove_reference_t<decltype(*m_begin)>;
            std::optional<result_type> m_result;

            void set_result()
            {
                if (m_begin != m_end) m_result = *m_begin;
            }

        public:
            using traits_type = iterator_impl_traits<result_type>;

            winrt_iterator_impl(It begin, It end) : m_begin(begin), m_end(end) { set_result(); }

            typename traits_type::reference value() { return *m_result; }

            void move_next()
            {
                ++m_begin;
                set_result();
            }

            bool is_valid() const { return m_begin != m_end; }
        };

        template <typename It>
        using winrt_iterator = iterator_base<winrt_iterator_impl<It>>;
    } // namespace impl

    template <typename Container, typename = decltype(std::declval<Container>().First())>
    auto to_iterable(Container&& container)
    {
        using It = decltype(winrt::impl::begin(container));
        return impl::iterable{ impl::winrt_iterator<It>{ impl::iterator_ctor, winrt::impl::begin(container), winrt::impl::end(container) } };
    }
} // namespace linq

#endif // LINQ_USE_WINRT

#endif // !LINQ_WINRT_HPP
