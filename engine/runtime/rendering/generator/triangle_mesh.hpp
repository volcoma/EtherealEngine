#ifndef GENERATOR_TRIANGLEMESH_HPP
#define GENERATOR_TRIANGLEMESH_HPP

#include "math.hpp"

#include "mesh_vertex.hpp"
#include "triangle.hpp"

namespace generator
{

/// A triangular mesh on the xy -plane.
/// @image html TriangleMesh.svg
class triangle_mesh_t
{
public:
	class triangles_t
	{
	public:
		bool done() const noexcept;
		triangle_t generate() const;
		void next();

	private:
		const triangle_mesh_t* mesh_;
		int row_;
		int col_;
		int i_;

		explicit triangles_t(const triangle_mesh_t& mesh);

		friend class triangle_mesh_t;
	};

	class vertices_t
	{
	public:
		bool done() const noexcept;
		mesh_vertex_t generate() const;
		void next();

	private:
		const triangle_mesh_t* mesh_;
		int row_;
		int col_;

		explicit vertices_t(const triangle_mesh_t& mesh);

		friend class triangle_mesh_t;
	};

	/// Makes a regular triangle centered at origin.
	/// @param radius The radius of the containing circle.
	/// @param segments The number of segments along each edge. Must be >= 1.
	explicit triangle_mesh_t(double radius = 1.0, int segments = 4);

	/// @param v0,v1,v2 The vertex positions of the triangle.
	/// @param segments The number of segments along each edge. Must be >= 1.
	triangle_mesh_t(const gml::dvec3& v0, const gml::dvec3& v1, const gml::dvec3& v2, int segments = 4);

	triangles_t triangles() const noexcept;

	vertices_t vertices() const noexcept;

private:
	gml::dvec3 v0_, v1_, v2_;

	gml::dvec3 normal_;

	int segments_;
};
}

#endif
