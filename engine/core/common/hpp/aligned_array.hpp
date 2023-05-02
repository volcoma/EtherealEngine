#pragma once
#include <iterator>
#include <type_traits>
#include <cstdint>

namespace hpp
{
template <typename T, std::size_t N, std::size_t A = N>
class aligned_array
{
public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // properly aligned uninitialized storage
    typename std::aligned_storage<sizeof(value_type) * N, A>::type storage;

    // Iterators.
    iterator begin() noexcept
    {
        return iterator(data());
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(data());
    }

    iterator end() noexcept
    {
        return iterator(data() + N);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(data() + N);
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_iterator cbegin() const noexcept
    {
        return const_iterator(data());
    }

    const_iterator cend() const noexcept
    {
        return const_iterator(data() + N);
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    // Capacity.
    constexpr size_type size() const noexcept
    {
        return N;
    }

    constexpr size_type max_size() const noexcept
    {
        return N;
    }

    constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    // Access an object in aligned storage
    const_reference operator[](std::size_t pos) const
    {
        return *(data() + pos);
    }
    reference operator[](std::size_t pos)
    {
        return *(data() + pos);
    }

    const_pointer data() const noexcept
    {
        return reinterpret_cast<const_pointer>(std::addressof(storage));
    }

    pointer data() noexcept
    {
        return reinterpret_cast<pointer>(std::addressof(storage));
    }
};
}
