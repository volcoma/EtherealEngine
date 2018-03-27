#include "circle_shape.hpp"

using namespace generator;

circle_shape_t::circle_shape_t(double radius, int segments, double start, double sweep)
	: parametric_shape_{[radius, start, sweep](double t) {
							const double angle = t * sweep + start;
							const double sine = std::sin(angle);
							const double cosine = std::cos(angle);

							shape_vertex_t vertex;
							vertex.position = gml::dvec2{radius * cosine, radius * sine};
							vertex.tangent = gml::dvec2{-sine, cosine};
							vertex.tex_coord = t;

							return vertex;
						},
						segments}
{
}
