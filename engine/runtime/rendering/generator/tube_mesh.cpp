#include "tube_mesh.hpp"

using namespace generator;

tube_mesh_t::tube_mesh_t(double radius, double innerRadius, double size, int slices, int segments,
						 double start, double sweep)
	: merge_mesh_{{radius, size, slices, segments, start, sweep},
				  {{{innerRadius, size, slices, segments, start, sweep}, true, false}}}
{
}
