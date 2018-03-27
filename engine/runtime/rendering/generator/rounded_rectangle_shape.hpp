#ifndef GENERATOR_ROUNDEDRECTANGLESHAPE_HPP
#define GENERATOR_ROUNDEDRECTANGLESHAPE_HPP

#include "circle_shape.hpp"
#include "line_shape.hpp"
#include "merge_shape.hpp"
#include "translate_shape.hpp"

namespace generator
{

/// Rectangle with rounded corners centered at origin aligned along the x and y axis.
class rounded_rectangle_shape_t
{
private:
	using impl_t =
		merge_shape_t<line_shape_t, translate_shape_t<circle_shape_t>, line_shape_t,
					  translate_shape_t<circle_shape_t>, line_shape_t, translate_shape_t<circle_shape_t>,
					  line_shape_t, translate_shape_t<circle_shape_t>>;
	impl_t merge_shape_;

public:
	/// @param radius Radius of the rounded corners.
	/// @param size Half of a length of an edge.
	/// @param slices Number of subdivisions in each rounded corner.
	/// @param segments Number of subdivisions along each edge.
	rounded_rectangle_shape_t(double radius = 0.25, const gml::dvec2& size = {0.75, 0.75}, int slices = 4,
							  const gml::ivec2& segments = {8, 8});

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
