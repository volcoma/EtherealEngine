#ifndef GENERATOR_PATHVERTEX_HPP
#define GENERATOR_PATHVERTEX_HPP

#include "math.hpp"

namespace generator
{

class path_vertex_t
{
public:
	/// Unit length vector perpendicular to the path at this point.
	/// Also the x-axis of the path coordinate system at this point.
	gml::dvec3 normal;

	gml::dvec3 position;

	/// Unit length vector parallel to the path at this point.
	/// Also the z-axis of the path at this point.
	gml::dvec3 tangent;

	double tex_coord;

	path_vertex_t()
		: normal{}
		, position{}
		, tangent{}
		, tex_coord{}
	{
	}

	/// Returns tangent x normal.
	/// Also the y-axis of the path coordinate system.
	/// See: http://mathworld.wolfram.com/BinormalVector.html
	gml::dvec3 binormal() const noexcept
	{
		return cross(tangent, normal);
	}
};
}

#endif
