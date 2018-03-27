#include "capped_cylinder_mesh.hpp"

using namespace generator;
using namespace generator::detail;

cap_t::cap_t(double radius, double distance, int slices, int rings, double start, double sweep)
	: translate_mesh_{{radius, 0.0, slices, rings, start, sweep}, {0.0, 0.0, distance}}
{
}

capped_cylinder_mesh_t::capped_cylinder_mesh_t(double radius, double size, int slices, int segments,
											   int rings, double start, double sweep)
	: merge_mesh_{{radius, size, slices, segments, start, sweep},
				  {radius, size, slices, rings, start, sweep},
				  {{{radius, -size, slices, rings, start, sweep}}, true, false}}
{
}
