/**CppLinq to_container.hpp
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
#ifndef LINQ_TO_CONTAINER_HPP
#define LINQ_TO_CONTAINER_HPP

#include <deque>
#include <linq/core.hpp>
#include <list>
#include <map>
#include <set>
#include <vector>

namespace linq
{
    template <typename T, typename Allocator = std::allocator<T>>
    constexpr auto to_list()
    {
        return [](auto e) {
            std::list<T, Allocator> result;
            for (auto item : e)
            {
                result.emplace_back(item);
            }
            return result;
        };
    }

    template <typename T, typename Comparer = std::less<T>, typename Allocator = std::allocator<T>>
    constexpr auto to_set()
    {
        return [](auto e) {
            std::set<T, Comparer, Allocator> result;
            for (auto item : e)
            {
                result.emplace(item);
            }
            return result;
        };
    }

    template <typename T, typename Allocator = std::allocator<T>>
    constexpr auto to_vector()
    {
        return [](auto e) {
            std::vector<T, Allocator> result;
            for (auto item : e)
            {
                result.emplace_back(item);
            }
            return result;
        };
    }

    template <typename T, typename Allocator = std::allocator<T>>
    constexpr auto to_deque()
    {
        return [](auto e) {
            std::deque<T, Allocator> result;
            for (auto item : e)
            {
                result.emplace_back(item);
            }
            return result;
        };
    }

    template <typename TKey, typename TElem, typename KeySelector, typename ElementSelector, typename Comparer = std::less<TKey>, typename Allocator = std::allocator<std::pair<const TKey, TElem>>>
    constexpr auto to_map(KeySelector&& keysel, ElementSelector&& elesel)
    {
        return [&](auto e) {
            std::map<TKey, TElem, Comparer, Allocator> result;
            for (auto item : e)
            {
                result.emplace(keysel(item), elesel(item));
            }
            return result;
        };
    }

    template <typename TKey, typename TElem, typename KeySelector, typename ElementSelector, typename Comparer = std::less<TKey>, typename Allocator = std::allocator<std::pair<const TKey, TElem>>>
    constexpr auto to_multimap(KeySelector&& keysel, ElementSelector&& elesel)
    {
        return [&](auto e) {
            std::multimap<TKey, TElem, Comparer, Allocator> result;
            for (auto item : e)
            {
                result.emplace(keysel(item), elesel(item));
            }
            return result;
        };
    }
} // namespace linq

#endif // !LINQ_TO_CONTAINER_HPP
