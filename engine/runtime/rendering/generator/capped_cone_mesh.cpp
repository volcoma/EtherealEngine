#include "capped_cone_mesh.hpp"

using namespace generator;

capped_cone_mesh_t::capped_cone_mesh_t(double radius, double size, int slices, int segments, int rings,
									   double start, double sweep)
	: merge_mesh_{{radius, size, slices, segments, start, sweep},
				  {{{disk_mesh_t{radius, 0.0, slices, rings}, gml::dvec3{0.0, 0.0, -size}}}, true, false}}
{
}
