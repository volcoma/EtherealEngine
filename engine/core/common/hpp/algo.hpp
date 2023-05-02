#pragma once
#include <algorithm>
#include <array>
#include <iterator>
#include <type_traits>
#include <vector>

namespace hpp
{
template<typename Iterator, typename Pred>
Iterator find_if_nth(Iterator first, Iterator last, size_t n, Pred closure)
{
    if(n == 0)
    {
        return last;
    }

    return std::find_if(first,
                        last,
                        [&](const auto& x)
                        {
                            return closure(x) && !(--n);
                        });
}

template<typename Iter, typename Pred>
auto index_of(Iter first, Iter last, Pred closure) -> typename std::iterator_traits<Iter>::difference_type
{
    return std::distance(first, std::find(first, last, closure));
}

template<typename Container, typename Pred>
auto next_circular(const Container& container, Pred closure) ->
    typename std::remove_reference<decltype(std::begin(std::declval<Container>()))>::type
{
    if(container.empty())
    {
        throw std::runtime_error("Container is empty");
    }

    auto iter = std::find_if(std::begin(container), std::end(container), closure);
    iter++;

    if(iter == std::end(container))
    {
        return std::begin(container);
    }
    return iter;
}

template<typename Container, typename Callback>
void for_each_2d(Container& container, Callback callback)
{
    for(size_t i = 0; i < container.size(); ++i)
    {
        for(size_t j = 0; j < container[i].size(); ++j)
        {
            callback(i, j, container[i][j]);
        }
    }
}

template<typename Container, typename Predicate>
void erase_if(Container& items, const Predicate& predicate)
{
    for(auto it = items.begin(); it != items.end();)
    {
        if(predicate(*it))
        {
            it = items.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

template<typename T>
auto transpose_2d(const std::vector<std::vector<T>>& items) -> std::vector<std::vector<T>>
{
    if(items.empty())
    {
        return {};
    }

    const auto row = items.size();
    const auto col = items.front().size();

    std::vector<std::vector<T>> result(col, std::vector<T>(row, T{}));
    for(size_t i = 0; i < row; ++i)
    {
        for(size_t j = 0; j < col; ++j)
        {
            result[j][i] = items[i][j];
        }
    }
    return result;
}

template<typename Container>
bool all_the_same(const Container& items)
{
    if(items.empty())
    {
        return true;
    }

    const auto& first = items.front();
    return std::all_of(items.begin(),
                       items.end(),
                       [&first](const auto& item)
                       {
                           return first == item;
                       });
}

} // namespace hpp
