#include "line_shape.hpp"

using namespace generator;

line_shape_t::line_shape_t(const gml::dvec2& start, const gml::dvec2& end, int segments)
	: parametric_shape_{[start, end](double t) {
							shape_vertex_t vertex;

							vertex.position = start + t * (end - start);
							vertex.tangent = normalize(end - start);
							vertex.tex_coord = t;

							return vertex;
						},
						segments}
{
}
