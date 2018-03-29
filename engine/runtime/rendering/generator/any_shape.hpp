#ifndef GENERATOR_ANYSHAPE_HPP
#define GENERATOR_ANYSHAPE_HPP

#include <memory>

#include "any_generator.hpp"
#include "edge.hpp"
#include "shape_vertex.hpp"

namespace generator
{

/// A type erasing container that can store any shape.
class any_shape
{
public:
	template <typename shape_t>
	any_shape(shape_t shape)
		: base_{new derived<shape_t>{std::move(shape)}}
	{
	}

	any_shape(const any_shape& that);

	any_shape(any_shape&&) = default;

	any_shape& operator=(const any_shape& that);

	any_shape& operator=(any_shape&&) = default;

	any_generator<edge_t> edges() const noexcept;

	any_generator<shape_vertex_t> vertices() const noexcept;

private:
	class base
	{
	public:
		virtual ~base();
		virtual std::unique_ptr<base> clone() const = 0;
		virtual any_generator<edge_t> edges() const = 0;
		virtual any_generator<shape_vertex_t> vertices() const = 0;
	};

	template <typename shape_t>
	class derived : public base
	{
	public:
		explicit derived(shape_t shape)
			: shape_(std::move(shape))
		{
		}

		std::unique_ptr<base> clone() const override
		{
			return std::unique_ptr<base>{new derived{shape_}};
		}

		any_generator<edge_t> edges() const override
		{
			return shape_.edges();
		}

		any_generator<shape_vertex_t> vertices() const override
		{
			return shape_.vertices();
		}

		shape_t shape_;
	};

	std::unique_ptr<base> base_;
};
}

#endif
