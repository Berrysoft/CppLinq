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
        template <typename Container, typename = decltype(winrt::impl::begin(std::declval<Container>())), typename = decltype(winrt::impl::end(std::declval<Container>()))>
        class winrt_iterable
        {
        private:
            Container m_container;

        public:
            winrt_iterable(Container&& container) : m_container(std::forward<Container>(container)) {}

            auto begin() const { return winrt::impl::begin(m_container); }
            auto end() const { return winrt::impl::end(m_container); }
        };
    } // namespace impl

    template <typename Container, typename Query, typename = decltype(winrt::impl::begin(std::declval<Container>())), typename = decltype(winrt::impl::end(std::declval<Container>()))>
    constexpr decltype(auto) operator>>(Container&& container, Query&& query)
    {
        return std::forward<Query>(query)(impl::winrt_iterable<Container>{ std::forward<Container>(container) });
    }
} // namespace linq

#endif // CPPWINRT_VERSION

#endif // !LINQ_WINRT_HPP
