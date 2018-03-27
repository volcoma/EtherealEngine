#include "torus_knot_mesh.hpp"

using namespace generator;

torus_knot_mesh_t::torus_knot_mesh_t(int p, int q, int slices, int segments)
	: extrude_mesh_{{0.25, slices, 0.0, gml::radians(360.0)}, {p, q, segments}}
{
}
