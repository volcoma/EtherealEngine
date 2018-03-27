#ifndef GENERATOR_MESHVERTEX_HPP
#define GENERATOR_MESHVERTEX_HPP

#include "math.hpp"

namespace generator
{

class mesh_vertex_t
{
public:
	gml::dvec3 position;

	/// Unit vector perpendicular to the surface.
	gml::dvec3 normal;

	/// UV texture coordinates
	gml::dvec2 tex_coord;

	mesh_vertex_t() noexcept
		: position{}
		, normal{}
		, tex_coord{}
	{
	}
};
}

#endif
