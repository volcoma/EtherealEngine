#include "capped_tube_mesh.hpp"

using namespace generator;

using namespace generator::detail;

tube_cap_t::tube_cap_t(double radius, double innerRadius, double distance, int slices, int rings,
					   double start, double sweep)
	: translate_mesh_{{radius, innerRadius, slices, rings, start, sweep}, {0.0, 0.0, distance}}
{
}

capped_tube_mesh_t::capped_tube_mesh_t(double radius, double innerRadius, double size, int slices,
									   int segments, int rings, double start, double sweep)
	: merge_mesh_{
		  {radius, innerRadius, size, slices, segments, start, sweep},
		  {radius, innerRadius, size, slices, rings, start, sweep},
		  {{radius, innerRadius, -size, slices, rings, start, sweep}},
	  }
{
}
