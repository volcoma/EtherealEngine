#include "helix_path.hpp"

using namespace generator;

helix_path_t::helix_path_t(double radius, double size, int segments, double start, double sweep)
	: parametric_path_{
		  [radius, size, start, sweep](double t) {
			  path_vertex_t vertex;
			  const double angle = start + t * sweep;
			  const double sine = std::sin(angle);
			  const double cosine = std::cos(angle);

			  vertex.position = gml::dvec3{radius * cosine, radius * sine, 2.0 * t * size - size};

			  vertex.tangent = gml::normalize(gml::dvec3{
				  -radius * sine,
				  radius * cosine,
				  2.0 * size / sweep,
			  });

			  vertex.normal = gml::dvec3{cosine, sine, 0.0};

			  vertex.tex_coord = t;

			  return vertex;
		  },
		  segments}
{
}
