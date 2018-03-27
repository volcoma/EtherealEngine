#ifndef GENERATOR_AXISSWAPSHAPE_HPP
#define GENERATOR_AXISSWAPSHAPE_HPP

#include "transform_shape.hpp"

namespace generator
{

/// Swaps the x and y axis.
template <typename shape_t>
class axis_swap_shape_t
{
private:
	using impl_t = transform_shape_t<shape_t>;
	impl_t transform_shape_;

public:
	/// @param shape Source data shape.
	axis_swap_shape_t(shape_t shape)
		: transform_shape_{std::move(shape), [](shape_vertex_t& vertex) {
							   std::swap(vertex.position[0u], vertex.position[1u]);
							   std::swap(vertex.tangent[0u], vertex.tangent[1u]);
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
axis_swap_shape_t<shape_t> axis_swap_shape(shape_t shape)
{
	return axis_swap_shape_t<shape_t>{std::move(shape)};
}
}

#endif
