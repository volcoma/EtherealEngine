#ifndef GENERATOR_DODECAHEDRON_HPP
#define GENERATOR_DODECAHEDRON_HPP

#include <array>
#include <memory>

#include "convex_polygon_mesh.hpp"
#include "mesh_vertex.hpp"
#include "triangle.hpp"
#include "utils.hpp"

namespace generator
{

/// A regular dodecahedron centered at origin with given radius.
/// Each face optionally subdivided along edges and/or radius.
/// @image html DodecahedronMesh.svg
class dodecahedron_mesh_t
{
public:
	class triangles_t
	{
	public:
		bool done() const noexcept;
		triangle_t generate() const;
		void next();

	private:
		const dodecahedron_mesh_t* mesh_;

		int face_index_;

		// Needs be a shared_ptr in order to make copy/move not to mess up the
		// internal pointer in triangles_.
		std::shared_ptr<const convex_polygon_mesh_t> face_mesh_;

		typename triangle_generator_type<convex_polygon_mesh_t>::type triangles_;

		explicit triangles_t(const dodecahedron_mesh_t& mesh) noexcept;

		friend class dodecahedron_mesh_t;
	};

	class vertices_t
	{
	public:
		bool done() const noexcept;
		mesh_vertex_t generate() const;
		void next();

	private:
		const dodecahedron_mesh_t* mesh_;

		int face_index_;

		// Needs be a shared_ptr in order to make copy/move not to mess up the
		// internal pointer in triangles_.
		std::shared_ptr<const convex_polygon_mesh_t> face_mesh_;

		typename vertex_generator_type<convex_polygon_mesh_t>::type vertices_;

		explicit vertices_t(const dodecahedron_mesh_t& mesh) noexcept;

		friend class dodecahedron_mesh_t;
	};

private:
	double radius_;

	int segments_;

	int rings_;

	int face_vertex_count_;

public:
	/// @param radius The radius of the enclosing sphere.
	/// @param segments The number segments along each edge. Should be >= 1.
	/// If <1 empty mesh is generated.
	/// @param rings The number of radial segments on each face. Should be >= 1.
	/// If <1 an empty mesh is generated.
	explicit dodecahedron_mesh_t(double radius = 1.0, int segments = 1, int rings = 1) noexcept;

	triangles_t triangles() const noexcept;

	vertices_t vertices() const noexcept;
};
}

#endif
