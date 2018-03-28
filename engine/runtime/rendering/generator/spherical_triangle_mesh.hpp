#ifndef GENERATOR_SPHERICALTRIANGLEMESH_HPP
#define GENERATOR_SPHERICALTRIANGLEMESH_HPP

#include "math.hpp"

#include "mesh_vertex.hpp"
#include "triangle.hpp"

namespace generator
{

/// A triangular region on a surface of a sphere.
/// @image html SphericalTriangleMesh.svg
class spherical_triangle_mesh_t
{
public:
	class triangles_t
	{
	public:
		bool done() const noexcept;

		triangle_t generate() const;

		void next();

	private:
		const spherical_triangle_mesh_t* mesh_;
		int row_;
		int col_;
		int i_;

		explicit triangles_t(const spherical_triangle_mesh_t& mesh);

		friend class spherical_triangle_mesh_t;
	};

	class vertices_t
	{
	public:
		bool done() const noexcept;

		mesh_vertex_t generate() const;

		void next();

	private:
		const spherical_triangle_mesh_t* mesh_;
		int row_;
		int col_;

		explicit vertices_t(const spherical_triangle_mesh_t& mesh);

		friend class spherical_triangle_mesh_t;
	};

	/// @param radius Radius of the containing sphere.
	/// @param segments Number of subdivisions along each edge.
	spherical_triangle_mesh_t(double radius = 1.0, int segments = 4);

	/// @param segments Number of subdivisions along each edge.
	spherical_triangle_mesh_t(const gml::dvec3& v0, const gml::dvec3& v1, const gml::dvec3& v2,
							  int segments = 4);

	triangles_t triangles() const noexcept;

	vertices_t vertices() const noexcept;

private:
	gml::dvec3 v0_, v1_, v2_;

	gml::dvec3 normal_;

	int segments_;
};
}

#endif
