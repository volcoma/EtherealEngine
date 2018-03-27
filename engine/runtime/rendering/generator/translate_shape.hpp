#ifndef GENERATOR_TRANSLATESHAPE_HPP
#define GENERATOR_TRANSLATESHAPE_HPP

#include "math.hpp"

#include "transform_shape.hpp"

namespace generator
{

/// Translates the position of each vertex by given amount.
template <typename shape_t>
class translate_shape_t
{
private:
	using impl_t = transform_shape_t<shape_t>;
	impl_t transform_shape_;

public:
	/// @param shape Source data shape.
	/// @param delta Amount to increment vertex positions.
	translate_shape_t(shape_t shape, const gml::dvec2& delta)
		: transform_shape_{std::move(shape), [delta](shape_vertex_t& value) { value.position += delta; }}
	{
	}

	using edges_t = typename impl_t::edges_t;

	edges_t edges() const noexcept
	{
		return transform_shape_.edges();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return transform_shape_.vertices();
	}
};

template <typename shape_t>
translate_shape_t<shape_t> translate_shape(shape_t shape, const gml::dvec2& delta)
{
	return translate_shape_t<shape_t>{std::move(shape), delta};
}
}

#endif
