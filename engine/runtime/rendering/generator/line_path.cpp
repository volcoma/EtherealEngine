#include "line_path.hpp"

using namespace generator;

line_path_t::line_path_t(const gml::dvec3& start, const gml::dvec3& end, const gml::dvec3& normal,
						 int segments)
	: parametric_path_{[start, end, normal](double t) {
						   path_vertex_t vertex;

						   vertex.position = start + t * (end - start);
						   vertex.tangent = normalize(end - start);
						   vertex.normal = normal;
						   vertex.tex_coord = t;

						   return vertex;
					   },
					   segments}
{
}
