// std::map-like class with an underlying vector
//
//                  DOCUMENTATION
//
// Simply include this file wherever you need.
// It defines the class hpp::flat_map, which is an almsot drop-in replacement
// of std::map. Flat map has an optional underlying container which by default
// is std::vector. Thus the items in the map are in a continuous block of
// memory. Thus iterating over the map is cache friendly, at the cost of
// O(n) for insert and erase.
//
// The elements inside (like in std::map) are kept in an order sorted by key.
// Getting a value by key is O(log2 n)
//
// It generally performs much faster than std::map for smaller sets of elements
//
// The difference with std::map, which makes flat_map an not-exactly-drop-in
// replacement is the last template argument:
// * std::map has <key, value, compare, allocator>
// * hpp::flat_map has <key, value, compare, container>
// The container must be an std::vector compatible type (hpp::static_vector
// is, for example, viable). The container value type must be
// std::pair<key, value>.
//
//                  Changing the allocator.
//
// If you want to change the allocator of flat map, you'll have to provide a
// container with the appropriate one. Example:
//
// hpp::flat_map<
//      string,
//      int,
//      less<string>,
//      std::vector<pair<string, int>, MyAllocator<pair<string, int>>
//  > mymap
//
//
//                  Configuration
//
// Throw
// Whether to throw exceptions: when `at` is called with a non-existent key.
// By default, like std::map, it throws an std::out_of_range exception. If you define
// HPP_FLAT_MAP_NO_THROW before including this header, the exception will
// be substituted by an assertion.
#pragma once

#include <vector>
#include <algorithm>
#include <type_traits>

#if !defined(HPP_FLAT_MAP_NO_THROW)
#   include <stdexcept>
#   define I_HPP_THROW_FLAT_MAP_OUT_OF_RANGE() throw std::out_of_range("hpp::flat_map out of range")
#else
#   include <cassert>
#   define I_HPP_THROW_FLAT_MAP_OUT_OF_RANGE() assert(false && "hpp::flat_map out of range")
#endif

namespace hpp
{

namespace fmimpl
{
struct less
{
    template <typename T, typename U>
    auto operator()(const T& t, const U& u) const -> decltype(t < u)
    {
        return t < u;
    }
};
}

template <typename Key, typename T, typename Compare = fmimpl::less, typename Container = std::vector<std::pair<Key, T>>>
class flat_map
{
public:
    typedef Key key_type;
    typedef T mapped_type;
    typedef std::pair<Key, T> value_type;
    typedef Container container_type;
    typedef Compare key_compare;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef typename container_type::allocator_type allocator_type;
    typedef typename std::allocator_traits<allocator_type>::pointer pointer;
    typedef typename std::allocator_traits<allocator_type>::pointer const_pointer;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;
    typedef typename container_type::reverse_iterator reverse_iterator;
    typedef typename container_type::const_reverse_iterator const_reverse_iterator;
    typedef typename container_type::difference_type difference_type;
    typedef typename container_type::size_type size_type;

    flat_map()
    {}

    explicit flat_map(const key_compare& comp, const allocator_type& alloc = allocator_type())
        : cmp_(comp)
        , container_(alloc)
    {}

    flat_map(std::initializer_list<value_type> init, const key_compare& comp = key_compare(), const allocator_type& alloc = allocator_type())
        : cmp_(comp)
        , container_(std::move(init), alloc)
    {
        std::sort(container_.begin(), container_.end(), cmp_);
        auto new_end = std::unique(container_.begin(), container_.end(), [this](const value_type& a, const value_type& b) {
            return !cmp_(a, b) && !cmp_(b, a);
        });
        container_.erase(new_end, container_.end());
    }

    flat_map(std::initializer_list<value_type> init, const allocator_type& alloc)
        : flat_map(std::move(init), key_compare(), alloc)
    {}

    flat_map(const flat_map& x) = default;
    flat_map& operator=(const flat_map& x) = default;

    flat_map(flat_map&& x) noexcept = default;
    flat_map& operator=(flat_map&& x) noexcept = default;

    iterator begin() noexcept { return container_.begin(); }
    const_iterator begin() const noexcept { return container_.begin(); }
    iterator end() noexcept { return container_.end(); }
    const_iterator end() const noexcept { return container_.end(); }
    reverse_iterator rbegin() noexcept { return container_.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return container_.rbegin(); }
    reverse_iterator rend() noexcept { return container_.rend(); }
    const_reverse_iterator rend() const noexcept { return container_.rend(); }
    const_iterator cbegin() const noexcept { return container_.cbegin(); }
    const_iterator cend() const noexcept { return container_.cend(); }

    bool empty() const noexcept { return container_.empty(); }
    size_type size() const noexcept { return container_.size(); }
    size_type max_size() const noexcept { return container_.max_size(); }

    void reserve(size_type count) { return container_.reserve(count); }
    size_type capacity() const noexcept { return container_.capacity(); }

    void clear() noexcept { container_.clear(); }

    template <typename K>
    iterator lower_bound(const K& k)
    {
        return std::lower_bound(container_.begin(), container_.end(), k, cmp_);
    }

    template <typename K>
    const_iterator lower_bound(const K& k) const
    {
        return std::lower_bound(container_.begin(), container_.end(), k, cmp_);
    }

    template <typename K>
    iterator upper_bound(const K& k)
    {
        return std::upper_bound(container_.begin(), container_.end(), k, cmp_);
    }

    template <typename K>
    const_iterator upper_bound(const K& k) const
    {
        return std::upper_bound(container_.begin(), container_.end(), k, cmp_);
    }

    template <typename K>
    std::pair<iterator, iterator> equal_range(const K& k)
    {
        return std::equal_range(container_.begin(), container_.end(), k, cmp_);
    }

    template <typename K>
    std::pair<const_iterator, const_iterator> equal_range(const K& k) const
    {
        return std::equal_range(container_.begin(), container_.end(), k, cmp_);
    }

    template <typename K>
    iterator find(const K& k)
    {
        auto i = lower_bound(k);
        if (i != end() && !cmp_(k, *i))
            return i;

        return end();
    }

    template <typename K>
    const_iterator find(const K& k) const
    {
        auto i = lower_bound(k);
        if (i != end() && !cmp_(k, *i))
            return i;

        return end();
    }

    template <typename K>
    size_t count(const K& k) const
    {
        return find(k) == end() ? 0 : 1;
    }

    template <typename P>
    std::pair<iterator, bool> insert(P&& val)
    {
        auto i = lower_bound(val.first);
        if (i != end() && !cmp_(val.first, *i))
        {
            return { i, false };
        }

        return{ container_.emplace(i, std::forward<P>(val)), true };
    }

    std::pair<iterator, bool> insert(const value_type& val)
    {
        auto i = lower_bound(val.first);
        if (i != end() && !cmp_(val.first, *i))
        {
            return { i, false };
        }

        return{ container_.emplace(i, val), true };
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        value_type val(std::forward<Args>(args)...);
        return insert(std::move(val));
    }

    iterator erase(const_iterator pos)
    {
        return container_.erase(pos);
    }

    iterator erase(iterator pos)
    {
        return container_.erase(const_iterator(pos));
    }

    template <typename K>
    size_type erase(const K& k)
    {
        auto i = find(k);
        if (i == end())
        {
            return 0;
        }

        erase(i);
        return 1;
    }

    template <typename K>
    typename std::enable_if<std::is_convertible<K, key_type>::value,
    mapped_type&>::type operator[](K&& k)
    {
        auto i = lower_bound(k);
        if (i != end() && !cmp_(k, *i))
        {
            return i->second;
        }

        i = container_.emplace(i, std::forward<K>(k), mapped_type());
        return i->second;
    }

    mapped_type& at(const key_type& k)
    {
        auto i = lower_bound(k);
        if (i == end() || cmp_(k, *i))
        {
            I_HPP_THROW_FLAT_MAP_OUT_OF_RANGE();
        }

        return i->second;
    }

    const mapped_type& at(const key_type& k) const
    {
        auto i = lower_bound(k);
        if (i == end() || cmp_(k, *i))
        {
            I_HPP_THROW_FLAT_MAP_OUT_OF_RANGE();
        }

        return i->second;
    }

    void swap(flat_map& x)
    {
        std::swap(cmp_, x.cmp_);
        container_.swap(x.container_);
    }

    const container_type& container() const noexcept
    {
        return container_;
    }

    // DANGER! If you're not careful with this function, you may irreversably break the map
    container_type& modify_container() noexcept
    {
        return container_;
    }

private:
    struct pair_compare
    {
        pair_compare() = default;
        pair_compare(const key_compare& kc) : kcmp(kc) {}
        bool operator()(const value_type& a, const value_type& b) const { return kcmp(a.first, b.first); }
        template <typename K> bool operator()(const value_type& a, const K& b) const { return kcmp(a.first, b); }
        template <typename K> bool operator()(const K& a, const value_type& b) const { return kcmp(a, b.first); }

        key_compare kcmp;
    };
    pair_compare cmp_;
    container_type container_;
};

template <typename Key, typename T, typename Compare, typename Container>
bool operator==(const flat_map<Key, T, Compare, Container>& a, const flat_map<Key, T, Compare, Container>& b)
{
    return a.container() == b.container();
}

template <typename Key, typename T, typename Compare, typename Container>
bool operator!=(const flat_map<Key, T, Compare, Container>& a, const flat_map<Key, T, Compare, Container>& b)
{
    return a.container() != b.container();
}

}
