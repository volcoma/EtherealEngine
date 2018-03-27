#include "knot_path.hpp"

using namespace generator;

namespace
{

gml::dvec3 knot(int p, int q, double t)
{
	t *= gml::radians(360.0);

	const double pt = p * t;
	const double qt = q * t;

	const double sinpt = std::sin(pt);
	const double cospt = std::cos(pt);
	const double sinqt = std::sin(qt);
	const double cosqt = std::cos(qt);

	const double r = 0.5 * (2.0 + sinqt);

	return gml::dvec3{r * cospt, r * sinpt, r * cosqt};
}
}

knot_path_t::knot_path_t(int p, int q, int segments)
	: parametric_path_{[p, q](double t) {
						   path_vertex_t vertex;

						   vertex.position = knot(p, q, t);

						   const gml::dvec3 prev = knot(p, q, t - 0.01);
						   const gml::dvec3 next = knot(p, q, t + 0.01);

						   vertex.tangent = normalize(next - prev);

						   vertex.normal = normalize(cross(next - prev, next + prev));

						   vertex.tex_coord = t;

						   return vertex;
					   },
					   segments}
{
}
