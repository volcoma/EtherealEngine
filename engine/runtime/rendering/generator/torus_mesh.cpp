#include "torus_mesh.hpp"

using namespace generator;

torus_mesh_t::torus_mesh_t(double minor, double major, int slices, int segments, double minorStart,
						   double minorSweep, double majorStart, double majorSweep)
	: axis_swap_mesh_{{{{minor, slices, minorStart + gml::radians(90.0), minorSweep}, {0.0, major}},
					   {1.0, 0.0},
					   segments,
					   majorStart,
					   majorSweep},
					  axis_t::Y,
					  axis_t::Z,
					  axis_t::X}
{
}
