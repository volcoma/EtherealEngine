#ifndef GENERATOR_SHAPETRANSLATOR_HPP
#define GENERATOR_SHAPETRANSLATOR_HPP

#include "shape_vertex.hpp"
#include "utils.hpp"
#include <functional>

namespace generator
{

/// Apply a mutator function to each vertex.
template <typename shape_t>
class transform_shape_t
{
private:
	using impl_t = shape_t;
	impl_t shape_;

public:
	class vertices_t
	{
	public:
		shape_vertex_t generate() const
		{
			auto temp = vertices_.generate();
			shape_->mutate_(temp);
			return temp;
		}

		bool done() const noexcept
		{
			return vertices_.done();
		}

		void next()
		{
			vertices_.next();
		}

	private:
		const transform_shape_t* shape_;

		typename vertex_generator_type<shape_t>::type vertices_;

		explicit vertices_t(const transform_shape_t& shape)
			: shape_{&shape}
			, vertices_{shape.shape_.vertices()}
		{
		}

		friend class transform_shape_t;
	};

	/// @param shape Source data shape.
	/// @param mutate Callback function that gets called once per vertex.
	transform_shape_t(shape_t shape, std::function<void(shape_vertex_t&)> mutate)
		: shape_{std::move(shape)}
		, mutate_{mutate}
	{
	}

	using edges_t = typename impl_t::edges_t;

	edges_t edges() const noexcept
	{
		return shape_.edges();
	}

	vertices_t vertices() const noexcept
	{
        return vertices_t{*this};
	}

private:
	std::function<void(shape_vertex_t&)> mutate_;
};

template <typename shape_t>
transform_shape_t<shape_t> transform_shape(shape_t shape, std::function<void(shape_vertex_t&)> mutate)
{
	return transform_shape_t<shape_t>{std::move(shape), std::move(mutate)};
}
}

#endif
