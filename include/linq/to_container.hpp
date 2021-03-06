/**CppLinq to_container.hpp
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
#ifndef LINQ_TO_CONTAINER_HPP
#define LINQ_TO_CONTAINER_HPP

#include <deque>
#include <linq/core.hpp>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace linq
{
    template <typename T, typename Allocator = std::allocator<T>>
    constexpr auto to_list(const Allocator& alloc = {})
    {
        return [alloc](auto&& container) {
            return std::list<T, Allocator>(std::begin(container), std::end(container), alloc);
        };
    }

    template <typename T, typename Comparer = std::less<T>, typename Allocator = std::allocator<T>>
    constexpr auto to_set(const Allocator& alloc = {})
    {
        return [alloc](auto&& container) {
            return std::set<T, Comparer, Allocator>(std::begin(container), std::end(container), alloc);
        };
    }

    template <typename T, typename Comparer = std::less<T>, typename Allocator = std::allocator<T>>
    constexpr auto to_multiset(const Allocator& alloc = {})
    {
        return [alloc](auto&& container) {
            return std::multiset<T, Comparer, Allocator>(std::begin(container), std::end(container), alloc);
        };
    }

    template <typename T, typename Hash = std::hash<T>, typename KeyEq = std::equal_to<T>, typename Allocator = std::allocator<T>>
    constexpr auto to_unordered_set(const Allocator& alloc = {})
    {
        return [alloc](auto&& container) {
            return std::unordered_set<T, Hash, KeyEq, Allocator>(std::begin(container), std::end(container), alloc);
        };
    }

    template <typename T, typename Hash = std::hash<T>, typename KeyEq = std::equal_to<T>, typename Allocator = std::allocator<T>>
    constexpr auto to_unordered_multiset(const Allocator& alloc = {})
    {
        return [alloc](auto&& container) {
            return std::unordered_multiset<T, Hash, KeyEq, Allocator>(std::begin(container), std::end(container), alloc);
        };
    }

    template <typename T, typename Allocator = std::allocator<T>>
    constexpr auto to_vector(const Allocator& alloc = {})
    {
        return [alloc](auto&& container) {
            return std::vector<T, Allocator>(std::begin(container), std::end(container), alloc);
        };
    }

    template <typename T, typename Allocator = std::allocator<T>>
    constexpr auto to_deque(const Allocator& alloc = {})
    {
        return [alloc](auto&& container) {
            return std::deque<T, Allocator>(std::begin(container), std::end(container), alloc);
        };
    }

    template <typename TKey, typename TElem, typename KeySelector, typename ElementSelector, typename Comparer = std::less<TKey>, typename Allocator = std::allocator<std::pair<const TKey, TElem>>>
    constexpr auto to_map(KeySelector&& keysel, ElementSelector&& elesel, const Allocator& alloc = {})
    {
        return [&, alloc](auto&& container) {
            std::map<TKey, TElem, Comparer, Allocator> result{ alloc };
            for (auto& item : container)
            {
                result.emplace(keysel(item), elesel(item));
            }
            return result;
        };
    }

    template <typename TKey, typename TElem, typename KeySelector, typename ElementSelector, typename Hash = std::hash<TKey>, typename KeyEq = std::equal_to<TKey>, typename Allocator = std::allocator<std::pair<const TKey, TElem>>>
    constexpr auto to_unordered_map(KeySelector&& keysel, ElementSelector&& elesel, const Allocator& alloc = {})
    {
        return [&, alloc](auto&& container) {
            std::unordered_map<TKey, TElem, Hash, KeyEq, Allocator> result{ alloc };
            for (auto& item : container)
            {
                result.emplace(keysel(item), elesel(item));
            }
            return result;
        };
    }

    template <typename TKey, typename TElem, typename KeySelector, typename ElementSelector, typename Comparer = std::less<TKey>, typename Allocator = std::allocator<std::pair<const TKey, TElem>>>
    constexpr auto to_multimap(KeySelector&& keysel, ElementSelector&& elesel, const Allocator& alloc = {})
    {
        return [&, alloc](auto&& container) {
            std::multimap<TKey, TElem, Comparer, Allocator> result{ alloc };
            for (auto& item : container)
            {
                result.emplace(keysel(item), elesel(item));
            }
            return result;
        };
    }

    template <typename TKey, typename TElem, typename KeySelector, typename ElementSelector, typename Hash = std::hash<TKey>, typename KeyEq = std::equal_to<TKey>, typename Allocator = std::allocator<std::pair<const TKey, TElem>>>
    constexpr auto to_unordered_multimap(KeySelector&& keysel, ElementSelector&& elesel, const Allocator& alloc = {})
    {
        return [&, alloc](auto&& container) {
            std::unordered_multimap<TKey, TElem, Hash, KeyEq, Allocator> result{ alloc };
            for (auto& item : container)
            {
                result.emplace(keysel(item), elesel(item));
            }
            return result;
        };
    }
} // namespace linq

#endif // !LINQ_TO_CONTAINER_HPP
