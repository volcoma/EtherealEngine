#ifndef GENERATOR_ROTATESHAPE_HPP
#define GENERATOR_ROTATESHAPE_HPP

#include "transform_shape.hpp"

namespace generator
{

/// Rotates a shape around the origin on the xy-plane.
template <typename shape_t>
class rotate_shape_t
{
private:
	using impl_t = transform_shape_t<shape_t>;
	impl_t transform_shape_;

public:
	/// @param shape Source data shape.
	/// @param angle Counterclockwise angle.
	rotate_shape_t(shape_t shape, double angle)
		: transform_shape_{std::move(shape), [angle](shape_vertex_t& value) {
							   auto rotation = gml::rotate(angle);
							   value.position = gml::transform(rotation, value.position);
							   value.tangent = gml::transform(rotation, value.tangent);
						   }}
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
rotate_shape_t<shape_t> rotate_shape(shape_t shape, double angle)
{
	return rotate_shape_t<shape_t>{std::move(shape), angle};
}
}

#endif
