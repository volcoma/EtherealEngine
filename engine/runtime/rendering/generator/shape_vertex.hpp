#ifndef GENERATOR_SHAPEGENERATOR_HPP
#define GENERATOR_SHAPEGENERATOR_HPP

#include "math.hpp"

namespace generator
{

/// A point on a path.
/// Position and assosiated coordinate system.
class shape_vertex_t
{
public:
	gml::dvec2 position;

	/// Unit length vector parallel to the shape at this point.
	/// Also the y-axis of the shape's local coordinate system.
	gml::dvec2 tangent;

	double tex_coord;

	shape_vertex_t()
		: position{}
		, tangent{}
		, tex_coord{}
	{
	}

	/// Returns the tangent rotated 90 dec clockwise.
	/// Also the x-axis of the shape's local coordinate system.
	gml::dvec2 normal() const noexcept
	{
		return -gml::cross(tangent);
	}
};
}

#endif
