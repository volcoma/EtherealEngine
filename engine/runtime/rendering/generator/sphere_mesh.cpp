#include "sphere_mesh.hpp"

using namespace generator;

sphere_mesh_t::sphere_mesh_t(double radius, int slices, int segments, double sliceStart, double sliceSweep,
							 double segmentStart, double segmentSweep)
	: axis_swap_mesh_{
		  {{radius, segments, segmentStart, segmentSweep}, {1.0, 0.0}, slices, sliceStart, sliceSweep},
		  axis_t::Y,
		  axis_t::Z,
		  axis_t::X}
{
}
