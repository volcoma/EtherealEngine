#ifndef GENERATOR_BEZIER_MESH_HPP
#define GENERATOR_BEZIER_MESH_HPP

#include <algorithm>
#include <limits>

#include "parametric_mesh.hpp"

namespace generator
{

/// A bezier patch with D0xD1 control points.
/// @tparam D0 Number of control points along the t[0] axis. Must be > 1.
/// @tparam D1 Number of control points along the t[1] axis. Must be > 1.
/// @image html BezierMesh.svg
template <int D0, int D1>
class bezier_mesh_t
{
private:
	static_assert(D0 > 1, "D0 must be > 1.");
	static_assert(D1 > 1, "D1 must be > 1.");

	using impl_t = parametric_mesh_t;
	impl_t parametric_mesh_;

	struct array_wrapper
	{
		gml::dvec3 data[D1][D0];

		array_wrapper(const gml::dvec3 (&p)[D1][D0])
		{
			std::copy(&p[0][0], &p[0][0] + D1 * D0, &data[0][0]);
		}
	};

	explicit bezier_mesh_t(const array_wrapper& p, const gml::ivec2& segments)
		: parametric_mesh_{[p](const gml::dvec2& t) {
							   mesh_vertex_t vertex;

							   vertex.position = gml::bezier2(p.data, t);

							   gml::dmat2x3 J = gml::bezier2Jacobian<1>(p.data, t);
							   vertex.normal = gml::cross(J[0], J[1]);

							   // If the normal was zero try a again near by.
							   const double e = std::numeric_limits<double>::epsilon();
							   if(dot(vertex.normal, vertex.normal) < e)
							   {
								   J = gml::bezier2Jacobian<1>(p.data, t + 10.0 * e);
								   vertex.normal = gml::cross(J[0], J[1]);
							   }
							   vertex.normal = gml::normalize(vertex.normal);

							   vertex.tex_coord = t;

							   return vertex;
						   },
						   segments}
	{
	}

public:
	/// @param p Control points
	/// @param segments Number of subdivisions along each axis
	explicit bezier_mesh_t(const gml::dvec3 (&p)[D1][D0], const gml::ivec2& segments = {16, 16})
		: // Work around a msvc lambda capture bug by wrapping the array.
		bezier_mesh_t{array_wrapper{p}, segments}
	{
	}

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return parametric_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return parametric_mesh_.vertices();
	}
};
}

#endif
