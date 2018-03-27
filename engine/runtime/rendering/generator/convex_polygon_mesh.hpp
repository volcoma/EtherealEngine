#ifndef GENERATOR_CONVEX_POLYGON_MESH_HPP
#define GENERATOR_CONVEX_POLYGON_MESH_HPP

#include <vector>

#include "math.hpp"

#include "mesh_vertex.hpp"
#include "triangle.hpp"

namespace generator
{

/// A polygonal disk with arbitrary number of corners.
/// Subdivided along each side to segments and radially to rings.
/// @image html ConvexPolygonMesh.svg
class convex_polygon_mesh_t
{
private:
	class triangles_t
	{
	public:
		bool done() const noexcept;
		triangle_t generate() const;
		void next();

	private:
		const convex_polygon_mesh_t* mesh_;

		bool odd_;

		int segment_index_;

		int side_index_;

		int ring_index_;

		explicit triangles_t(const convex_polygon_mesh_t&) noexcept;

		friend class convex_polygon_mesh_t;
	};

	class vertices_t
	{
	public:
		bool done() const noexcept;
		mesh_vertex_t generate() const;
		void next();

	private:
		const convex_polygon_mesh_t* mesh_;

		bool center_done_;

		int segment_index_;

		int side_index_;

		int ring_index_;

		explicit vertices_t(const convex_polygon_mesh_t&) noexcept;

		friend class convex_polygon_mesh_t;
	};

	std::vector<gml::dvec3> vertices_;

	int segments_;

	int rings_;

	gml::dvec3 center_;

	gml::dvec3 normal_;

	gml::dvec3 tangent_;

	gml::dvec3 bitangent_;

	gml::dvec2 tex_delta_;

public:
	/// @param radius The radius the enclosing circle.
	/// @param sides The number of sides. Should be >= 3. If <3 an empty mesh
	/// is generated.
	/// @param segments The number of segments per side. Should be >= 1. If zero
	/// an empty mesh is generated.
	/// @param rings The number of radial segments. Should be >= 1. = yelds an empty mesh.
	explicit convex_polygon_mesh_t(double radius = 1.0, int sides = 5, int segments = 4,
								   int rings = 4) noexcept;

	//// @param vertices The corner vertex coordinates. Should form a convex polygon.
	explicit convex_polygon_mesh_t(const std::vector<gml::dvec2>& vertices, int segments = 1,
								   int rings = 1) noexcept;

	/// @param vertices The corner vertex coordinates. Should be coplanar and
	/// form a convex polygon.
	/// calculated as an avarage.
	explicit convex_polygon_mesh_t(std::vector<gml::dvec3> vertices, int segments = 1,
								   int rings = 1) noexcept;

	triangles_t triangles() const noexcept;

	vertices_t vertices() const noexcept;
};
}

#endif
