#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace nonstd
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

	friend class ring_iterator<type, false>;
	friend class ring_iterator<type, true>;

	template <class ContiguousIterator>
	ring_span(ContiguousIterator begin, ContiguousIterator end, Popper p = Popper()) noexcept;

	template <class ContiguousIterator>
	ring_span(ContiguousIterator begin, ContiguousIterator end, ContiguousIterator first, size_type size,
			  Popper p = Popper()) noexcept;

	ring_span(ring_span&&) = default;
	ring_span& operator=(ring_span&&) = default;

	bool empty() const noexcept;
	bool full() const noexcept;
	size_type size() const noexcept;
	size_type capacity() const noexcept;

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

	void swap(type& rhs) noexcept; // (std::is_nothrow_swappable<Popper>::value);

	// Example implementation
private:
	reference at(size_type idx) noexcept;
	const_reference at(size_type idx) const noexcept;
	size_type back_idx() const noexcept;
	void increase_size() noexcept;

	T* m_data;
	size_type m_size;
	size_type m_capacity;
	size_type m_front_idx;
	Popper m_popper;
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
	using size_type = typename Ring::size_type;
	ring_iterator(size_type idx, std::conditional_t<is_const, const Ring, Ring>* rv) noexcept;
	size_type m_idx;
	std::conditional_t<is_const, const Ring, Ring>* m_rv;
};

template <typename Ring, bool C>
ring_iterator<Ring, C> operator+(ring_iterator<Ring, C> it, std::ptrdiff_t) noexcept;

template <typename Ring, bool C>
ring_iterator<Ring, C> operator-(ring_iterator<Ring, C> it, std::ptrdiff_t) noexcept;
} // namespace nonstd

// Sample implementation

template <typename T>
void nonstd::null_popper<T>::operator()(T&) const noexcept
{
}

template <typename T>
T nonstd::default_popper<T>::operator()(T& t) const
{
	return std::move(t);
}

template <typename T>
nonstd::copy_popper<T>::copy_popper(T t)
	: m_copy(std::move(t))
{
}

template <typename T>
T nonstd::copy_popper<T>::operator()(T& t) const
{
	T old = m_copy;
	using std::swap;
	swap(old, t);
	return old;
}

template <typename T, class Popper>
template <class ContiguousIterator>
nonstd::ring_span<T, Popper>::ring_span(ContiguousIterator begin, ContiguousIterator end, Popper p) noexcept
	: m_data(&*begin)
	, m_size(0)
	, m_capacity(end - begin)
	, m_front_idx(0)
	, m_popper(std::move(p))
{
}

template <typename T, class Popper>
template <class ContiguousIterator>
nonstd::ring_span<T, Popper>::ring_span(ContiguousIterator begin, ContiguousIterator end,
										ContiguousIterator first, size_type size, Popper p) noexcept
	: m_data(&*begin)
	, m_size(size)
	, m_capacity(end - begin)
	, m_front_idx(first - begin)
	, m_popper(std::move(p))
{
}

template <typename T, class Popper>
bool nonstd::ring_span<T, Popper>::empty() const noexcept
{
	return m_size == 0;
}

template <typename T, class Popper>
bool nonstd::ring_span<T, Popper>::full() const noexcept
{
	return m_size == m_capacity;
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::size_type nonstd::ring_span<T, Popper>::size() const noexcept
{
	return m_size;
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::size_type nonstd::ring_span<T, Popper>::capacity() const noexcept
{
	return m_capacity;
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::reference nonstd::ring_span<T, Popper>::front() noexcept
{
	return *begin();
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::reference nonstd::ring_span<T, Popper>::back() noexcept
{
	return *(--end());
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::const_reference nonstd::ring_span<T, Popper>::front() const noexcept
{
	return *begin();
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::const_reference nonstd::ring_span<T, Popper>::back() const noexcept
{
	return *(--end());
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::iterator nonstd::ring_span<T, Popper>::begin() noexcept
{
	return iterator(m_front_idx, this);
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::const_iterator nonstd::ring_span<T, Popper>::begin() const noexcept
{
	return const_iterator(m_front_idx, this);
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::iterator nonstd::ring_span<T, Popper>::end() noexcept
{
	return iterator(size() + m_front_idx, this);
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::const_iterator nonstd::ring_span<T, Popper>::end() const noexcept
{
	return const_iterator(size() + m_front_idx, this);
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::const_iterator nonstd::ring_span<T, Popper>::cbegin() const noexcept
{
	return begin();
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::reverse_iterator nonstd::ring_span<T, Popper>::rbegin() noexcept
{
	return reverse_iterator(end());
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::const_reverse_iterator nonstd::ring_span<T, Popper>::rbegin() const
	noexcept
{
	return const_reverse_iterator(end());
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::const_reverse_iterator nonstd::ring_span<T, Popper>::crbegin() const
	noexcept
{
	return const_reverse_iterator(end());
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::const_iterator nonstd::ring_span<T, Popper>::cend() const noexcept
{
	return end();
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::reverse_iterator nonstd::ring_span<T, Popper>::rend() noexcept
{
	return reverse_iterator(begin());
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::const_reverse_iterator nonstd::ring_span<T, Popper>::rend() const
	noexcept
{
	return const_reverse_iterator(begin());
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::const_reverse_iterator nonstd::ring_span<T, Popper>::crend() const
	noexcept
{
	return const_reverse_iterator(begin());
}

template <typename T, class Popper>
template <bool b, typename>
void nonstd::ring_span<T, Popper>::push_back(const T& value) noexcept(
	std::is_nothrow_copy_assignable<T>::value)
{
	m_data[back_idx()] = value;
	increase_size();
}

template <typename T, class Popper>
template <bool b, typename>
void nonstd::ring_span<T, Popper>::push_back(T&& value) noexcept(std::is_nothrow_move_assignable<T>::value)
{
	m_data[back_idx()] = std::move(value);
	increase_size();
}

template <typename T, class Popper>
template <class... FromType>
void nonstd::ring_span<T, Popper>::emplace_back(FromType&&... from_value) noexcept(
	std::is_nothrow_constructible<T, FromType...>::value&& std::is_nothrow_move_assignable<T>::value)
{
	m_data[back_idx()] = T(std::forward<FromType>(from_value)...);
	increase_size();
}

template <typename T, class Popper>
auto nonstd::ring_span<T, Popper>::pop_front()
{
	assert(m_size != 0);
	auto old_front_idx = m_front_idx;
	m_front_idx = (m_front_idx + 1) % m_capacity;
	--m_size;
	return m_popper(m_data[old_front_idx]);
}

template <typename T, class Popper>
void nonstd::ring_span<T, Popper>::swap(
	nonstd::ring_span<T, Popper>& rhs) noexcept //(std::is_nothrow_swappable<Popper>::value)
{
	using std::swap;
	swap(m_data, rhs.m_data);
	swap(m_size, rhs.m_size);
	swap(m_capacity, rhs.m_capacity);
	swap(m_front_idx, rhs.m_front_idx);
	swap(m_popper, rhs.m_popper);
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::reference nonstd::ring_span<T, Popper>::at(size_type i) noexcept
{
	return m_data[i % m_capacity];
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::const_reference nonstd::ring_span<T, Popper>::at(size_type i) const
	noexcept
{
	return m_data[i % m_capacity];
}

template <typename T, class Popper>
typename nonstd::ring_span<T, Popper>::size_type nonstd::ring_span<T, Popper>::back_idx() const noexcept
{
	return (m_front_idx + m_size) % m_capacity;
}

template <typename T, class Popper>
void nonstd::ring_span<T, Popper>::increase_size() noexcept
{
	if(++m_size > m_capacity)
	{
		m_size = m_capacity;
		m_front_idx = (m_front_idx + 1) % m_capacity;
	}
}

template <typename Ring, bool is_const>
nonstd::ring_iterator<Ring, is_const>::operator nonstd::ring_iterator<Ring, true>() const noexcept
{
	return nonstd::ring_iterator<Ring, true>(m_idx, m_rv);
}

template <typename Ring, bool is_const>
template <bool C>
bool nonstd::ring_iterator<Ring, is_const>::operator==(const nonstd::ring_iterator<Ring, C>& rhs) const
	noexcept
{
	return (m_idx == rhs.m_idx) && (m_rv == rhs.m_rv);
}

template <typename Ring, bool is_const>
template <bool C>
bool nonstd::ring_iterator<Ring, is_const>::operator!=(const nonstd::ring_iterator<Ring, C>& rhs) const
	noexcept
{
	return !(*this == rhs);
}

template <typename Ring, bool is_const>
template <bool C>
bool nonstd::ring_iterator<Ring, is_const>::operator<(const nonstd::ring_iterator<Ring, C>& rhs) const
	noexcept
{
	return (m_idx < rhs.m_idx) && (m_rv == rhs.m_rv);
}

template <typename Ring, bool is_const>
template <bool C>
bool nonstd::ring_iterator<Ring, is_const>::operator<=(const nonstd::ring_iterator<Ring, C>& rhs) const
	noexcept
{
	return !(rhs < *this);
}

template <typename Ring, bool is_const>
template <bool C>
bool nonstd::ring_iterator<Ring, is_const>::operator>(const nonstd::ring_iterator<Ring, C>& rhs) const
	noexcept
{
	return (rhs < *this);
}

template <typename Ring, bool is_const>
template <bool C>
bool nonstd::ring_iterator<Ring, is_const>::operator>=(const nonstd::ring_iterator<Ring, C>& rhs) const
	noexcept
{
	return !(*this < rhs);
}

template <typename Ring, bool is_const>
typename nonstd::ring_iterator<Ring, is_const>::reference nonstd::ring_iterator<Ring, is_const>::
operator*() const noexcept
{
	return m_rv->at(m_idx);
}

template <typename Ring, bool is_const>
nonstd::ring_iterator<Ring, is_const>& nonstd::ring_iterator<Ring, is_const>::operator++() noexcept
{
	++m_idx;
	return *this;
}

template <typename Ring, bool is_const>
nonstd::ring_iterator<Ring, is_const> nonstd::ring_iterator<Ring, is_const>::operator++(int)noexcept
{
	auto r(*this);
	++*this;
	return r;
}

template <typename Ring, bool is_const>
nonstd::ring_iterator<Ring, is_const>& nonstd::ring_iterator<Ring, is_const>::operator--() noexcept
{
	--m_idx;
	return *this;
}

template <typename Ring, bool is_const>
nonstd::ring_iterator<Ring, is_const> nonstd::ring_iterator<Ring, is_const>::operator--(int)noexcept
{
	auto r(*this);
	--*this;
	return r;
}

template <typename Ring, bool is_const>
nonstd::ring_iterator<Ring, is_const>& nonstd::ring_iterator<Ring, is_const>::
operator+=(std::ptrdiff_t i) noexcept
{
	this->m_idx += i;
	return *this;
}

template <typename Ring, bool is_const>
nonstd::ring_iterator<Ring, is_const>& nonstd::ring_iterator<Ring, is_const>::
operator-=(std::ptrdiff_t i) noexcept
{
	this->m_idx -= i;
	return *this;
}

template <typename Ring, bool is_const>
template <bool C>
std::ptrdiff_t nonstd::ring_iterator<Ring, is_const>::
operator-(const nonstd::ring_iterator<Ring, C>& rhs) const noexcept
{
	return static_cast<std::ptrdiff_t>(this->m_idx) - static_cast<std::ptrdiff_t>(rhs.m_idx);
}

template <typename Ring, bool is_const>
nonstd::ring_iterator<Ring, is_const>::ring_iterator(
	typename nonstd::ring_iterator<Ring, is_const>::size_type idx,
	std::conditional_t<is_const, const Ring, Ring>* rv) noexcept
	: m_idx(idx)
	, m_rv(rv)
{
}

namespace nonstd
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
} // namespace nonstd
