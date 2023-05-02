#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <set>

namespace hpp
{
template <typename T>
struct null_popper
{
    void operator()(T&) const noexcept;
};

template <typename T>
struct default_popper
{
    T operator()(T& t) const;
};

template <typename T>
struct copy_popper
{
    copy_popper(T t);
    T operator()(T& t) const;

private:
    T m_copy;
};

template <typename, bool>
class ring_iterator;

template <typename T, class Popper = default_popper<T>>
class ring_span
{
public:
    using type = ring_span<T, Popper>;
    using size_type = std::size_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_reference = const T&;
    using iterator = ring_iterator<type, false>;
    using const_iterator = ring_iterator<type, true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using changes_container_type = std::set<size_type>;

    friend class ring_iterator<type, false>;
    friend class ring_iterator<type, true>;
    ring_span() = default;
    template <class ContiguousIterator>
    ring_span(ContiguousIterator begin, ContiguousIterator end, Popper p = Popper()) noexcept;

    template <class ContiguousIterator>
    ring_span(ContiguousIterator begin, ContiguousIterator end, ContiguousIterator first, size_type size,
              Popper p = Popper()) noexcept;

    bool empty() const noexcept;
    bool full() const noexcept;
    size_type size() const noexcept;
    size_type capacity() const noexcept;
    size_type front_idx() const noexcept;

    reference front() noexcept;
    const_reference front() const noexcept;
    reference back() noexcept;
    const_reference back() const noexcept;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;

    const_iterator cbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_reverse_iterator crend() const noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;

    template <bool b = true, typename = std::enable_if_t<b && std::is_copy_assignable<T>::value>>
    void push_back(const value_type& from_value) noexcept(std::is_nothrow_copy_assignable<T>::value);
    template <bool b = true, typename = std::enable_if_t<b && std::is_move_assignable<T>::value>>
    void push_back(value_type&& from_value) noexcept(std::is_nothrow_move_assignable<T>::value);
    template <class... FromType>
    void emplace_back(FromType&&... from_value) noexcept(
        std::is_nothrow_constructible<T, FromType...>::value&& std::is_nothrow_move_assignable<T>::value);
    auto pop_front();
    auto pop_back();

    void swap(type& rhs) noexcept; // (std::is_nothrow_swappable<Popper>::value);

    const changes_container_type& changes() const noexcept;
    void mark_as_changed();
    void clear_changes() noexcept;
    void set_recording_changes(bool recording) noexcept;
    size_type clamp_index(std::ptrdiff_t idx) const;
    // Example implementation
private:
    void on_change(size_type idx);
    reference at(std::ptrdiff_t idx) noexcept;
    const_reference at(std::ptrdiff_t idx) const noexcept;


    //returns the NEXT back index, i.e one past the last inserted.
    size_type back_idx() const noexcept;
    void increase_size() noexcept;

    T* data_ = nullptr;
    size_type size_ = 0;
    size_type capacity_ = 0;
    size_type front_idx_ = 0;
    Popper popper_;
    changes_container_type changes_{};
    bool recording_changes_{};

};

template <typename T, class Popper>
void swap(ring_span<T, Popper>&, ring_span<T, Popper>&) noexcept;

template <typename Ring, bool is_const>
class ring_iterator
{
public:
    using type = ring_iterator<Ring, is_const>;
    using value_type = typename Ring::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = typename std::conditional_t<is_const, const value_type, value_type>*;
    using reference = typename std::conditional_t<is_const, const value_type, value_type>&;
    using iterator_category = std::random_access_iterator_tag;

    operator ring_iterator<Ring, true>() const noexcept;

    template <bool C>
    bool operator==(const ring_iterator<Ring, C>& rhs) const noexcept;
    template <bool C>
    bool operator!=(const ring_iterator<Ring, C>& rhs) const noexcept;
    template <bool C>
    bool operator<(const ring_iterator<Ring, C>& rhs) const noexcept;
    template <bool C>
    bool operator<=(const ring_iterator<Ring, C>& rhs) const noexcept;
    template <bool C>
    bool operator>(const ring_iterator<Ring, C>& rhs) const noexcept;
    template <bool C>
    bool operator>=(const ring_iterator<Ring, C>& rhs) const noexcept;

    reference operator*() const noexcept;
    type& operator++() noexcept;
    type operator++(int)noexcept;
    type& operator--() noexcept;
    type operator--(int)noexcept;

    type& operator+=(std::ptrdiff_t i) noexcept;
    type& operator-=(std::ptrdiff_t i) noexcept;

    template <bool C>
    std::ptrdiff_t operator-(const ring_iterator<Ring, C>& rhs) const noexcept;

    // Example implementation
private:
    friend Ring;
    friend class ring_iterator<Ring, !is_const>;
    using size_type = std::ptrdiff_t;
    ring_iterator(size_type idx, std::conditional_t<is_const, const Ring, Ring>* rv) noexcept;
    size_type m_idx;
    std::conditional_t<is_const, const Ring, Ring>* m_rv;
};

template <typename Ring, bool C>
ring_iterator<Ring, C> operator+(ring_iterator<Ring, C> it, std::ptrdiff_t) noexcept;

template <typename Ring, bool C>
ring_iterator<Ring, C> operator-(ring_iterator<Ring, C> it, std::ptrdiff_t) noexcept;
} // namespace hpp

// Sample implementation

template <typename T>
void hpp::null_popper<T>::operator()(T&) const noexcept
{
}

template <typename T>
T hpp::default_popper<T>::operator()(T& t) const
{
    return std::move(t);
}

template <typename T>
hpp::copy_popper<T>::copy_popper(T t)
    : m_copy(std::move(t))
{
}

template <typename T>
T hpp::copy_popper<T>::operator()(T& t) const
{
    T old = m_copy;
    using std::swap;
    swap(old, t);
    return old;
}

template <typename T, class Popper>
template <class ContiguousIterator>
hpp::ring_span<T, Popper>::ring_span(ContiguousIterator begin, ContiguousIterator end, Popper p) noexcept
    : data_(&*begin),
      capacity_(end - begin),
      popper_(std::move(p))
{
}

template <typename T, class Popper>
template <class ContiguousIterator>
hpp::ring_span<T, Popper>::ring_span(ContiguousIterator begin, ContiguousIterator end,
                                     ContiguousIterator first, size_type size, Popper p) noexcept
    : data_(&*begin),
      size_(size),
      capacity_(end - begin),
      front_idx_(first - begin),
      popper_(std::move(p))
{
}

template <typename T, class Popper>
bool hpp::ring_span<T, Popper>::empty() const noexcept
{
    return size_ == 0;
}

template <typename T, class Popper>
bool hpp::ring_span<T, Popper>::full() const noexcept
{
    return size_ == capacity_;
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::size_type hpp::ring_span<T, Popper>::size() const noexcept
{
    return size_;
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::size_type hpp::ring_span<T, Popper>::capacity() const noexcept
{
    return capacity_;
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::size_type hpp::ring_span<T, Popper>::front_idx() const noexcept
{
    return front_idx_;
}
template <typename T, class Popper>
const typename hpp::ring_span<T, Popper>::changes_container_type& hpp::ring_span<T, Popper>::changes() const noexcept
{
    return changes_;
}

template<typename T, class Popper>
void hpp::ring_span<T, Popper>::mark_as_changed()
{
    set_recording_changes(true);
    for(auto& el : *this)
    {
        (void)el;
    }
    set_recording_changes(false);
}

template <typename T, class Popper>
void hpp::ring_span<T, Popper>::clear_changes() noexcept
{
    changes_.clear();
}

template<typename T, class Popper>
void hpp::ring_span<T, Popper>::set_recording_changes(bool recording) noexcept
{
    recording_changes_ = recording;
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::reference hpp::ring_span<T, Popper>::front() noexcept
{
    return *begin();
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::reference hpp::ring_span<T, Popper>::back() noexcept
{
    return *(--end());
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::const_reference hpp::ring_span<T, Popper>::front() const noexcept
{
    return *begin();
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::const_reference hpp::ring_span<T, Popper>::back() const noexcept
{
    return *(--end());
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::iterator hpp::ring_span<T, Popper>::begin() noexcept
{
    auto idx = front_idx_;
    return iterator(idx, this);
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::const_iterator hpp::ring_span<T, Popper>::begin() const noexcept
{
    return const_iterator(front_idx_, this);
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::iterator hpp::ring_span<T, Popper>::end() noexcept
{
    auto idx = size() + front_idx_;
    return iterator(idx, this);
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::const_iterator hpp::ring_span<T, Popper>::end() const noexcept
{
    return const_iterator(size() + front_idx_, this);
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::const_iterator hpp::ring_span<T, Popper>::cbegin() const noexcept
{
    return begin();
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::reverse_iterator hpp::ring_span<T, Popper>::rbegin() noexcept
{
    return reverse_iterator(end());
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::const_reverse_iterator hpp::ring_span<T, Popper>::rbegin() const noexcept
{
    return const_reverse_iterator(end());
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::const_reverse_iterator hpp::ring_span<T, Popper>::crbegin() const noexcept
{
    return const_reverse_iterator(end());
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::const_iterator hpp::ring_span<T, Popper>::cend() const noexcept
{
    return end();
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::reverse_iterator hpp::ring_span<T, Popper>::rend() noexcept
{
    return reverse_iterator(begin());
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::const_reverse_iterator hpp::ring_span<T, Popper>::rend() const noexcept
{
    return const_reverse_iterator(begin());
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::const_reverse_iterator hpp::ring_span<T, Popper>::crend() const noexcept
{
    return const_reverse_iterator(begin());
}

template <typename T, class Popper>
template <bool b, typename>
void hpp::ring_span<T, Popper>::push_back(const T& value) noexcept(std::is_nothrow_copy_assignable<T>::value)
{
    auto idx = back_idx();
    data_[idx] = value;
    on_change(idx);
    increase_size();
}

template <typename T, class Popper>
template <bool b, typename>
void hpp::ring_span<T, Popper>::push_back(T&& value) noexcept(std::is_nothrow_move_assignable<T>::value)
{
    auto idx = back_idx();
    data_[idx] = std::move(value);
    on_change(idx);
    increase_size();
}

template <typename T, class Popper>
template <class... FromType>
void hpp::ring_span<T, Popper>::emplace_back(FromType&&... from_value) noexcept(
    std::is_nothrow_constructible<T, FromType...>::value&& std::is_nothrow_move_assignable<T>::value)
{
    auto idx = back_idx();
    data_[idx] = T(std::forward<FromType>(from_value)...);
    on_change(idx);
    increase_size();
}

template <typename T, class Popper>
auto hpp::ring_span<T, Popper>::pop_front()
{
    assert(size_ != 0);
    auto old_front_idx = front_idx_;
    front_idx_ = (front_idx_ + 1) % capacity_;
    --size_;
    return popper_(data_[old_front_idx]);
}

template <typename T, class Popper>
auto hpp::ring_span<T, Popper>::pop_back()
{
    assert(size_ != 0);
    // back_idx returns one past the current back
    auto old_back_idx = back_idx() - 1;
    --size_;
    return popper_(data_[old_back_idx]);
}

template <typename T, class Popper>
void hpp::ring_span<T, Popper>::on_change(size_type idx)
{
    if(recording_changes_)
    {
        changes_.emplace(idx);
    }
}

template <typename T, class Popper>
void hpp::ring_span<T, Popper>::swap(
    hpp::ring_span<T, Popper>& rhs) noexcept //(std::is_nothrow_swappable<Popper>::value)
{
    using std::swap;
    swap(data_, rhs.data_);
    swap(size_, rhs.size_);
    swap(capacity_, rhs.capacity_);
    swap(front_idx_, rhs.front_idx_);
    swap(popper_, rhs.popper_);
    swap(changes_, rhs.changes_);
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::reference hpp::ring_span<T, Popper>::at(std::ptrdiff_t i) noexcept
{
    auto idx = clamp_index(i);
    on_change(idx);
    return data_[idx];
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::const_reference hpp::ring_span<T, Popper>::at(std::ptrdiff_t i) const noexcept
{
    auto idx = clamp_index(i);
    return data_[idx];
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::size_type hpp::ring_span<T, Popper>::clamp_index(std::ptrdiff_t idx) const
{
    return (capacity_ + (idx % std::ptrdiff_t(capacity_))) % std::ptrdiff_t(capacity_);
}

template <typename T, class Popper>
typename hpp::ring_span<T, Popper>::size_type hpp::ring_span<T, Popper>::back_idx() const noexcept
{
    return (front_idx_ + size_) % capacity_;
}

template <typename T, class Popper>
void hpp::ring_span<T, Popper>::increase_size() noexcept
{
    if(++size_ > capacity_)
    {
        size_ = capacity_;
        front_idx_ = (front_idx_ + 1) % capacity_;
    }
}

template <typename Ring, bool is_const>
hpp::ring_iterator<Ring, is_const>::operator hpp::ring_iterator<Ring, true>() const noexcept
{
    return hpp::ring_iterator<Ring, true>(m_idx, m_rv);
}

template <typename Ring, bool is_const>
template <bool C>
bool hpp::ring_iterator<Ring, is_const>::operator==(const hpp::ring_iterator<Ring, C>& rhs) const noexcept
{
    return (m_idx == rhs.m_idx) && (m_rv == rhs.m_rv);
}

template <typename Ring, bool is_const>
template <bool C>
bool hpp::ring_iterator<Ring, is_const>::operator!=(const hpp::ring_iterator<Ring, C>& rhs) const noexcept
{
    return !(*this == rhs);
}

template <typename Ring, bool is_const>
template <bool C>
bool hpp::ring_iterator<Ring, is_const>::operator<(const hpp::ring_iterator<Ring, C>& rhs) const noexcept
{
    return (m_idx < rhs.m_idx) && (m_rv == rhs.m_rv);
}

template <typename Ring, bool is_const>
template <bool C>
bool hpp::ring_iterator<Ring, is_const>::operator<=(const hpp::ring_iterator<Ring, C>& rhs) const noexcept
{
    return !(rhs < *this);
}

template <typename Ring, bool is_const>
template <bool C>
bool hpp::ring_iterator<Ring, is_const>::operator>(const hpp::ring_iterator<Ring, C>& rhs) const noexcept
{
    return (rhs < *this);
}

template <typename Ring, bool is_const>
template <bool C>
bool hpp::ring_iterator<Ring, is_const>::operator>=(const hpp::ring_iterator<Ring, C>& rhs) const noexcept
{
    return !(*this < rhs);
}

template <typename Ring, bool is_const>
typename hpp::ring_iterator<Ring, is_const>::reference hpp::ring_iterator<Ring, is_const>::operator*() const
    noexcept
{
    return m_rv->at(m_idx);
}

template <typename Ring, bool is_const>
hpp::ring_iterator<Ring, is_const>& hpp::ring_iterator<Ring, is_const>::operator++() noexcept
{
    ++m_idx;
    return *this;
}

template <typename Ring, bool is_const>
hpp::ring_iterator<Ring, is_const> hpp::ring_iterator<Ring, is_const>::operator++(int)noexcept
{
    auto r(*this);
    ++*this;
    return r;
}

template <typename Ring, bool is_const>
hpp::ring_iterator<Ring, is_const>& hpp::ring_iterator<Ring, is_const>::operator--() noexcept
{
    --m_idx;
    return *this;
}

template <typename Ring, bool is_const>
hpp::ring_iterator<Ring, is_const> hpp::ring_iterator<Ring, is_const>::operator--(int)noexcept
{
    auto r(*this);
    --*this;
    return r;
}

template <typename Ring, bool is_const>
hpp::ring_iterator<Ring, is_const>& hpp::ring_iterator<Ring, is_const>::operator+=(std::ptrdiff_t i) noexcept
{
    this->m_idx += i;
    return *this;
}

template <typename Ring, bool is_const>
hpp::ring_iterator<Ring, is_const>& hpp::ring_iterator<Ring, is_const>::operator-=(std::ptrdiff_t i) noexcept
{
    this->m_idx -= i;
    return *this;
}

template <typename Ring, bool is_const>
template <bool C>
std::ptrdiff_t hpp::ring_iterator<Ring, is_const>::operator-(const hpp::ring_iterator<Ring, C>& rhs) const
    noexcept
{
    return static_cast<std::ptrdiff_t>(this->m_idx) - static_cast<std::ptrdiff_t>(rhs.m_idx);
}

template <typename Ring, bool is_const>
hpp::ring_iterator<Ring, is_const>::ring_iterator(typename hpp::ring_iterator<Ring, is_const>::size_type idx,
                                                  std::conditional_t<is_const, const Ring, Ring>* rv) noexcept
    : m_idx(idx),
      m_rv(rv)
{
}

namespace hpp
{
template <typename T, class Popper>
void swap(ring_span<T, Popper>& a, ring_span<T, Popper>& b) noexcept
{
    a.swap(b);
}

template <typename Ring, bool C>
ring_iterator<Ring, C> operator+(ring_iterator<Ring, C> it, std::ptrdiff_t i) noexcept
{
    it += i;
    return it;
}

template <typename Ring, bool C>
ring_iterator<Ring, C> operator-(ring_iterator<Ring, C> it, std::ptrdiff_t i) noexcept
{
    it -= i;
    return it;
}

} // namespace hpp
