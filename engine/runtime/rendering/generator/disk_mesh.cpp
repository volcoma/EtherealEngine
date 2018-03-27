#include "disk_mesh.hpp"

using namespace generator;

disk_mesh_t::disk_mesh_t(double radius, double innerRadius, int slices, int rings, double start, double sweep)
	: axis_swap_mesh_{{{{0.0, innerRadius}, {0.0, radius}, rings}, {1.0, 0.0}, slices, start, sweep},
					  axis_t::Y,
					  axis_t::Z,
					  axis_t::X}
{
}
