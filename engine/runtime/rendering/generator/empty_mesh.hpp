#ifndef GENERATOR_EMPTYMESH_HPP
#define GENERATOR_EMPTYMESH_HPP

#include "iterator.hpp"
#include "mesh_vertex.hpp"
#include "triangle.hpp"

namespace generator
{

/// Empty Mesh with zero vertices and triangles.
class empty_mesh
{
public:
	class triangles_t
	{
	public:
		triangle_t generate() const;
		bool done() const noexcept;
		void next();

	private:
		triangles_t();

		friend class empty_mesh;
	};

	class vertices_t
	{
	public:
		mesh_vertex_t generate() const;
		bool done() const noexcept;
		void next();

	private:
		vertices_t();

		friend class empty_mesh;
	};

	empty_mesh();

	triangles_t triangles() const noexcept;

	vertices_t vertices() const noexcept;
};
}

#endif
