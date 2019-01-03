#ifndef LINQ_AGGREGATE_HPP
#define LINQ_AGGREGATE_HPP

#include <linq/core.hpp>

namespace linq
{
    namespace impl
    {
        constexpr auto _allways_true()
        {
            return [](auto) { return true; };
        }
    } // namespace impl

    template <typename Pred = decltype(impl::_allways_true())>
    constexpr auto count(Pred&& pred = impl::_allways_true())
    {
        return [&](auto e) {
            std::size_t result = 0;
            for (auto item : e)
            {
                if (pred(item))
                    ++result;
            }
            return result;
        };
    }

    template <typename Pred>
    constexpr auto all(Pred&& pred)
    {
        return [&](auto e) {
            for (auto item : e)
            {
                if (!pred(item))
                    return false;
            }
            return true;
        };
    }

    template <typename Pred>
    constexpr auto any(Pred&& pred)
    {
        return [&](auto e) {
            for (auto item : e)
            {
                if (pred(item))
                    return true;
            }
            return false;
        };
    }
} // namespace linq

#endif // !LINQ_AGGREGATE_HPP
