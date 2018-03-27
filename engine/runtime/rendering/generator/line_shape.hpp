#ifndef GENERATOR_LINESHAPE_HPP
#define GENERATOR_LINESHAPE_HPP

#include "parametric_shape.hpp"

namespace generator
{

/// A line from a point to a point.
/// @image html LineShape.svg
class line_shape_t
{
private:
	using impl_t = parametric_shape_t;
	impl_t parametric_shape_;

public:
	/// @param start Start position
	/// @param end End position
	/// @param segments Number of subdivisions
	line_shape_t(const gml::dvec2& start = {0.0, -1.0}, const gml::dvec2& end = {0.0, 1.0}, int segments = 8);

	using edges_t = typename impl_t::edges_t;

	edges_t edges() const noexcept
	{
		return parametric_shape_.edges();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return parametric_shape_.vertices();
	}
};
}

#endif /* LINE_HPP_ */
