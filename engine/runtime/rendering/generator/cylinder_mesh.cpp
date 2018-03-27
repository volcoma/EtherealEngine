#include "cylinder_mesh.hpp"

using namespace generator;

cylinder_mesh_t::cylinder_mesh_t(double radius, double size, int slices, int segments, double start,
								 double sweep)
	: axis_swap_mesh_{{{{size, radius}, {-size, radius}, segments}, {1.0, 0.0}, slices, start, sweep},
					  axis_t::Y,
					  axis_t::Z,
					  axis_t::X}
{
}
