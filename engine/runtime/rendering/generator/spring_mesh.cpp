#include "spring_mesh.hpp"

using namespace generator;

spring_mesh_t::spring_mesh_t(double minor, double major, double size, int slices, int segments,
							 double minorStart, double minorSweep, double majorStart, double majorSweep)
	: extrude_mesh_{{minor, slices, minorStart, minorSweep}, {major, size, segments, majorStart, majorSweep}}
{
}
