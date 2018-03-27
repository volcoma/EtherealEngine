#ifndef GENERATOR_AXISSWAPPATH_HPP
#define GENERATOR_AXISSWAPPATH_HPP

#include "axis.hpp"
#include "transform_path.hpp"

namespace generator
{

/// Swaps axis in path.
template <typename path_t>
class axis_swap_path_t
{
private:
	using impl_t = transform_path_t<path_t>;
	impl_t transform_path_;

public:
	/// @param path Source data path
	/// @param x Axis to use as the X-axis
	/// @param y Axis to use as the Y-axis
	/// @param z Axis to use as the Z-axis
	axis_swap_path_t(path_t path, axis_t x, axis_t y, axis_t z)
		: transform_path_{std::move(path), [x, y, z](path_vertex_t& vertex) {
							  vertex.position = gml::dvec3{vertex.position[static_cast<int>(x)],
														   vertex.position[static_cast<int>(y)],
														   vertex.position[static_cast<int>(z)]};
							  vertex.tangent = gml::dvec3{vertex.tangent[static_cast<int>(x)],
														  vertex.tangent[static_cast<int>(y)],
														  vertex.tangent[static_cast<int>(z)]};
							  vertex.normal = gml::dvec3{vertex.normal[static_cast<int>(x)],
														 vertex.normal[static_cast<int>(y)],
														 vertex.normal[static_cast<int>(z)]};
						  }}
	{
	}

	using edges_t = typename impl_t::edges_t;

	edges_t edges() const noexcept
	{
		return transform_path_.edges();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return transform_path_.vertices();
	}
};

template <typename path_t>
axis_swap_path_t<path_t> axis_swap_path(path_t path, axis_t x, axis_t y, axis_t z)
{
	return axis_swap_path_t<path_t>{std::move(path), x, y, z};
}
}

#endif
