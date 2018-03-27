#include "capsule_mesh.hpp"

using namespace generator;

capsule_mesh_t::capsule_mesh_t(double radius, double size, int slices, int segments, int rings, double start,
							   double sweep)
	: merge_mesh_{
		  {radius, size, slices, segments, start, sweep},
		  {{radius, slices, rings, start, sweep, 0.0, gml::radians(90.0)}, {0.0, 0.0, size}},
		  {{radius, slices, rings, start, sweep, gml::radians(90.0), gml::radians(90.0)}, {0.0, 0.0, -size}}}
{
}
