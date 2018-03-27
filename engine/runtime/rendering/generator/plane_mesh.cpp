#include "plane_mesh.hpp"

using namespace generator;

plane_mesh_t::plane_mesh_t(const gml::dvec2& size, const gml::ivec2& segments)
	: parametric_mesh_{[size](const gml::dvec2& t) {
						  mesh_vertex_t vertex;
						  vertex.position = gml::dvec3{-size + 2.0 * t * size, 0.0};
						  vertex.normal = gml::dvec3{0.0, 0.0, 1.0};
						  vertex.tex_coord = t;
						  return vertex;
					  },
					  segments}
{
}
