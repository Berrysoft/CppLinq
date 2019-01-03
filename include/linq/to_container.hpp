#ifndef LINQ_TO_CONTAINER_HPP
#define LINQ_TO_CONTAINER_HPP

#include <linq/core.hpp>
#include <list>
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
} // namespace linq

#endif // !LINQ_TO_CONTAINER_HPP
