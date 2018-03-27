#ifndef GENERATOR_RECTANGLESHAPE_HPP
#define GENERATOR_RECTANGLESHAPE_HPP

#include "line_shape.hpp"
#include "merge_shape.hpp"

namespace generator
{

/// Rectangle centered at origin aligned along the x and y axis.
/// @image html RectangleShape.svg
class rectangle_shape_t
{
private:
	using impl_t = merge_shape_t<line_shape_t, line_shape_t, line_shape_t, line_shape_t>;
	impl_t merge_shape_;

public:
	/// @param size Half of the length of an edge.
	/// @param segments Number of subdivisions per edge.
	rectangle_shape_t(const gml::dvec2& size = gml::dvec2{1.0, 1.0},
					  const gml::ivec2& segments = gml::ivec2{8, 8});

	using edges_t = typename impl_t::edges_t;

	edges_t edges() const noexcept
	{
		return merge_shape_.edges();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return merge_shape_.vertices();
	}
};
}

#endif
