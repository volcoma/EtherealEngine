#ifndef GENERATOR_ITERATOR_HPP
#define GENERATOR_ITERATOR_HPP

#include <cassert>
#include <stdexcept>

#include "utils.hpp"

namespace generator
{

/// An iterator that can be used to "drive" a generator.
/// Note that iterator mutates the generator.
template <typename generator_t>
class iterator_t
{
private:
	generator_t* generator_;

	typename generated_type<generator_t>::type value_;

public:
	using iterator_category = std::input_iterator_tag;

	using value_type = typename generated_type<generator_t>::type;

	using difference_type = std::ptrdiff_t;

	using pointer = value_type*;

	using reference = value_type&;

	/// Creates a dummy end iterator.
	iterator_t() noexcept
		: generator_{nullptr}
		, value_{}
	{
	}

	/// Iterator to the given generator.
	iterator_t(generator_t& generator) noexcept
		: generator_{&generator}
		, value_{}
	{
		if(generator_->done())
			generator_ = nullptr;
		else
			value_ = generator_->generate();
	}

	/// Advance the iterator.
	/// Might make the iterator "out of range".
	/// @throws std::out_of_range If the iterator is out of range.
	iterator_t& operator++()
	{
		if(!generator_)
			throw std::out_of_range("Iterator out of range!");
		generator_->next();
		if(generator_->done())
			generator_ = nullptr;
		else
			value_ = generator_->generate();
		return *this;
	}

	/// Get reference to the current generated value.
	/// @throws std::out_of_range If the iterator is out of range.
	const typename iterator_t::value_type& operator*() const
	{
		if(!generator_)
			throw std::out_of_range("Iterator out of range!");
		return value_;
	}

	/// Get pointer to the current generated value.
	/// @throws std::out_of_range If the iterator is out of range
	const typename iterator_t::value_type* operator->() const
	{
		if(!generator_)
			throw std::out_of_range("Iterator out of range!");
		return &value_;
	}

	/// Iterators are equal if both are out of range or both iterate the same
	/// generator.
	bool operator==(const iterator_t& that) const noexcept
	{
		return generator_ == that.generator_;
	}

	bool operator!=(const iterator_t& that) const noexcept
	{
		return generator_ != that.generator_;
	}
};

/// Will return an iterator to the generator.
template <typename generator_t>
iterator_t<generator_t> begin(generator_t& generator) noexcept
{
	return iterator_t<generator_t>{generator};
}

/// Returns a dummy end iterator
template <typename generator_t>
iterator_t<generator_t> end(const generator_t&) noexcept
{
	return iterator_t<generator_t>{};
}
}

#endif
