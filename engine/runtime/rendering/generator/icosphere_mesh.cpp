#include "icosphere_mesh.hpp"

using namespace generator;

ico_sphere_mesh_t::ico_sphere_mesh_t(double radius, int segments)
	: spherify_mesh_{{1.0, segments}, radius, 1.0}
{
}
