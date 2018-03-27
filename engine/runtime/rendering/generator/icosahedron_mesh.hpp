#ifndef GENERATOR_ICOSAHEDRONMESH_HPP
#define GENERATOR_ICOSAHEDRONMESH_HPP

#include <array>
#include <memory>

#include "mesh_vertex.hpp"
#include "triangle.hpp"
#include "triangle_mesh.hpp"
#include "utils.hpp"

namespace generator
{

/// Regular icosahedron centered at origin with given radius.
/// @image html IcosahedronMesh.svg
class icosahedron_mesh_t
{
public:
	class triangles_t
	{
	public:
		bool done() const noexcept;
		triangle_t generate() const;
		void next();

	private:
		const icosahedron_mesh_t* mesh_;

		int i_;

		// Needs be a shared_ptr in order to make copy/move not to mess up the
		// internal pointer in triangles_.
		std::shared_ptr<const triangle_mesh_t> triangle_mesh_;

		typename triangle_generator_type<triangle_mesh_t>::type triangles_;

		triangles_t(const icosahedron_mesh_t& mesh);

		friend class icosahedron_mesh_t;
	};

	class vertices_t
	{
	public:
		bool done() const noexcept;
		mesh_vertex_t generate() const;
		void next();

	private:
		const icosahedron_mesh_t* mesh_;

		int i_;

		// Needs be a shared_ptr in order to make copy/move not to mess up the
		// internal pointer in triangles_.
		std::shared_ptr<const triangle_mesh_t> triangle_mesh_;

		typename vertex_generator_type<triangle_mesh_t>::type vertices_;

		vertices_t(const icosahedron_mesh_t& mesh);

		friend class icosahedron_mesh_t;
	};

private:
	double radius_;

	int segments_;

	int face_vertex_count_;

public:
	/// @param radius The radius of the enclosing sphere.
	/// @param segments The number segments along each edge. Must be >= 1.
	icosahedron_mesh_t(double radius = 1.0, int segments = 1);

	triangles_t triangles() const noexcept;

	vertices_t vertices() const noexcept;
};
}

#endif
