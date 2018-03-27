#ifndef GENERATOR_CIRCLESHAPE_HPP
#define GENERATOR_CIRCLESHAPE_HPP

#include "parametric_shape.hpp"

namespace generator
{

/// A circle centered at origin.
/// @image html CircleShape.svg
class circle_shape_t
{
private:
	using impl_t = parametric_shape_t;
	impl_t parametric_shape_;

public:
	/// @param radius Radius of the circle
	/// @param segments Number of subdivisions around the circle.
	/// @param start Counterclockwise angle relative to x-axis.
	/// @param sweep Counterclockwise angle.
	circle_shape_t(double radius = 1.0, int segments = 32, double start = 0.0,
				   double sweep = gml::radians(360.0));

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

#endif
