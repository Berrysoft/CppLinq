/**CppLinq winrt.hpp
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
#ifndef LINQ_WINRT_HPP
#define LINQ_WINRT_HPP

#ifdef CPPWINRT_VERSION

#include <linq/core.hpp>

namespace linq
{
    namespace impl
    {
        template <typename Collection, typename Iter = decltype(std::declval<Collection>().First())>
        class winrt_container_enumerator
        {
        private:
            Collection m_collection;
            Iter m_iter;

        public:
            constexpr winrt_container_enumerator(Collection const& collection) : m_collection(collection), m_iter(m_collection.First()) {}

            constexpr operator bool() const { return m_iter.HasCurrent(); }
            constexpr winrt_container_enumerator& operator++()
            {
                m_iter.MoveNext();
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_iter.Current(); }
        };

        template <typename Collection, typename = std::enable_if_t<winrt::impl::has_GetAt<Collection>::value>>
        class winrt_fast_container_enumerator
        {
        private:
            Collection m_collection;
            std::uint32_t m_index, m_size;

        public:
            constexpr winrt_fast_container_enumerator(Collection const& collection) : m_collection(collection), m_index(0), m_size(m_collection.Size()) {}

            constexpr operator bool() const { return m_index < m_size; }
            constexpr winrt_fast_container_enumerator& operator++()
            {
                ++m_index;
                return *this;
            }
            constexpr decltype(auto) operator*() { return m_collection.GetAt(m_index); }
        };
    } // namespace impl

    // SFINAE to determine whether the container has GetAt member function.
    template <typename C, typename = void>
    constexpr bool has_GetAt_v{ false };

    template <typename C>
    constexpr bool has_GetAt_v<C, std::void_t<decltype(std::declval<C>().GetAt(0))>>{ true };

    template <typename Collection, typename = decltype(std::declval<Collection>().First())>
    constexpr auto get_enumerator(Collection&& c)
    {
        if constexpr (has_GetAt_v<Collection>)
        {
            return impl::winrt_fast_container_enumerator<Collection>(c);
        }
        else
        {
            return impl::winrt_container_enumerator<Collection>(c);
        }
    }

    template <typename Collection, typename Query, typename = decltype(std::declval<Collection>().First())>
    constexpr auto operator>>(Collection&& c, Query&& q)
    {
        return std::forward<Query>(q)(enumerable(get_enumerator<Collection>(std::forward<Collection>(c))));
    }
} // namespace linq

#endif // CPPWINRT_VERSION

#endif // !LINQ_WINRT_HPP
