#pragma once

#include "ring_view.hpp"
#include <array>
#include <vector>

namespace nonstd
{

template <typename T, std::size_t N>
struct policy_std_array
{
	using buffer_t = typename std::array<T, N>;
	using size_type = typename buffer_t::size_type;
	using value_type = typename buffer_t::value_type;

	static buffer_t create_with_size(size_type /*n*/)
	{
		return {};
	}
};

template <typename T>
struct policy_std_vector
{
	using buffer_t = typename std::vector<T>;
	using size_type = typename buffer_t::size_type;
	using value_type = typename buffer_t::value_type;

	static buffer_t create_with_size(size_type n)
	{
		return buffer_t(n);
	}
};

template <typename policy_t>
class ring_buffer : public ring_span<typename policy_t::value_type>
{
public:
	using container_type = typename policy_t::buffer_t;
	using value_type = typename policy_t::value_type;
	using size_type = typename policy_t::size_type;
	using view_type = ring_span<value_type>;
	using policy_type = policy_t;

	explicit ring_buffer(size_type size = 0) noexcept(std::is_nothrow_default_constructible<container_type>::value)
		: container_(policy_type::create_with_size(size))
		, view_type(std::begin(container_), std::end(container_), std::begin(container_), 0)
	{
	}

	ring_buffer(const ring_buffer& rhs) noexcept(std::is_nothrow_copy_constructible<container_type>::value)
		: ring_buffer(&*std::begin(rhs) - &*std::begin(rhs.container_), rhs.size(), rhs.container_)
	{
	}

	ring_buffer(ring_buffer&& rhs) noexcept(std::is_nothrow_move_constructible<container_type>::value)
		: ring_buffer(&*std::begin(rhs) - &*std::begin(rhs.container_), rhs.size(), std::move(rhs.container_))
	{
	}

	explicit ring_buffer(const container_type& rhs) noexcept(std::is_nothrow_copy_constructible<container_type>::value)
		: ring_buffer(0, rhs.size(), rhs)
	{
	}

	explicit ring_buffer(container_type&& rhs) noexcept(std::is_nothrow_move_constructible<container_type>::value)
		: ring_buffer(0, rhs.size(), std::move(rhs))
	{
	}

	ring_buffer&
	operator=(const ring_buffer& rhs) noexcept(std::is_nothrow_copy_assignable<container_type>::value)
	{
		const auto first_idx = (&*std::begin(rhs) - &*std::begin(rhs.container_));
		container_ = rhs.container_;
		static_cast<view_type&>(*this) = view_type(std::begin(container_), std::end(container_),
												   std::begin(container_) + first_idx, rhs.size());
		return *this;
	}

	ring_buffer& operator=(ring_buffer&& rhs) noexcept(std::is_nothrow_copy_assignable<container_type>::value)
	{
		const auto first_idx = (&*std::begin(rhs) - &*std::begin(rhs.container_));
		container_ = std::move(rhs.container_);
		static_cast<view_type&>(*this) = view_type(std::begin(container_), std::end(container_),
												   std::begin(container_) + first_idx, rhs.size());
		return *this;
	}

	void clear() noexcept
	{
		static_cast<view_type&>(*this) =
			view_type(std::begin(container_), std::end(container_), std::begin(container_), 0);
	}

private:
	template <class... Args>
	ring_buffer(size_type first_idx, size_type size,
				Args&&... args) noexcept(noexcept(container_type(std::forward<Args>(args)...)))
		: container_(std::forward<Args>(args)...)
		, view_type(std::begin(container_), std::end(container_), std::begin(container_) + first_idx, size)
	{
	}

	container_type container_;
};

template <typename T, size_t N>
using stack_ringbuffer = ring_buffer<policy_std_array<T, N>>;

template <typename T>
using heap_ringbuffer = ring_buffer<policy_std_vector<T>>;

} // namespace nonstd
