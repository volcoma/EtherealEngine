#include "cone_mesh.hpp"

using namespace generator;

cone_mesh_t::cone_mesh_t(double radius, double size, int slices, int segments, double start, double sweep)
	: axis_swap_mesh_{{{{size, 0.0}, {-size, radius}, segments}, {1.0, 0.0}, slices, start, sweep},
					  axis_t::Y,
					  axis_t::Z,
					  axis_t::X}
{
}
