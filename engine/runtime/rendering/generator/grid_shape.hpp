#ifndef GENERATOR_GRIDSHAPE_HPP
#define GENERATOR_GRIDSHAPE_HPP

#include "edge.hpp"
#include "line_shape.hpp"
#include "merge_shape.hpp"
#include "repeat_shape.hpp"
#include "shape_vertex.hpp"

namespace generator
{

/**
 * A 2d regular grid.
 */
class grid_shape_t
{
private:
	using impl_t = merge_shape_t<repeat_shape_t<line_shape_t>, repeat_shape_t<line_shape_t>>;

	impl_t mImpl;

public:
	using edges_t = impl_t::edges_t;

	using vertices_t = impl_t::vertices_t;

	/// @param size A half of the side length of the grid.
	/// @param segments The Number of cells in the grid.
	/// If <1 an empty shape results.
	/// @param subSegments The number of segment along each cell edge.
	/// If <1 an empty shape results.
	explicit grid_shape_t(const gml::dvec2& size = {1.0, 1.0}, const gml::ivec2& segments = {4, 4},
						  const gml::ivec2& subSegments = {2, 2}) noexcept;

	edges_t edges() const noexcept;

	vertices_t vertices() const noexcept;
};
}

#endif
