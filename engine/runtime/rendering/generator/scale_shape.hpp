#ifndef GENERATOR_SCALESHAPE_HPP
#define GENERATOR_SCALESHAPE_HPP

#include "transform_shape.hpp"

namespace generator
{

/// Scales a shape keeping tangents unit length.
template <typename shape_t>
class scale_shape_t
{
private:
	using impl_t = transform_shape_t<shape_t>;
	impl_t transform_shape_;

public:
	/// @param shape Source data shape.
	/// @param scale Scale factor.
	scale_shape_t(shape_t shape, const gml::dvec2& scale)
		: transform_shape_{std::move(shape), [scale](shape_vertex_t& value) {
							   value.position *= scale;
							   value.tangent = normalize(scale * value.tangent);
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
scale_shape_t<shape_t> scale_shape(shape_t shape, const gml::dvec2& scale)
{
	return scale_shape_t<shape_t>{std::move(shape), scale};
}
}

#endif
