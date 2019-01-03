#ifndef LINQ_TO_CONTAINER_HPP
#define LINQ_TO_CONTAINER_HPP

#include <linq/core.hpp>
#include <list>
#include <map>
#include <set>
#include <vector>

namespace linq
{
    template <typename T>
    constexpr auto to_list()
    {
        return [](auto e) {
            std::list<T> result;
            for (auto item : e)
            {
                result.emplace_back(item);
            }
            return result;
        };
    }

    template <typename T>
    constexpr auto to_set()
    {
        return [](auto e) {
            std::set<T> result;
            for (auto item : e)
            {
                result.emplace(item);
            }
            return result;
        };
    }

    template <typename T>
    constexpr auto to_vector()
    {
        return [](auto e) {
            std::vector<T> result;
            for (auto item : e)
            {
                result.emplace_back(item);
            }
            return result;
        };
    }

    template <typename KeySelector, typename ElementSelector>
    constexpr auto to_map(KeySelector&& keysel, ElementSelector&& elesel)
    {
        return [&](auto e) {
            using TItem = decltype(*e.enumerator());
            using TKey = decltype(keysel(std::declval<TItem>()));
            using TElement = decltype(elesel(std::declval<TItem>()));
            std::map<TKey, TElement> result;
            for (auto item : e)
            {
                result.emplace(keysel(item), elesel(item));
            }
            return result;
        };
    }
} // namespace linq

#endif // !LINQ_TO_CONTAINER_HPP
