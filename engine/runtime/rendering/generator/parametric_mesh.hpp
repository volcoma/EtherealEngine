#ifndef GENERATOR_PARAMETRICMESH_HPP
#define GENERATOR_PARAMETRICMESH_HPP

#include <functional>

#include "mesh_vertex.hpp"
#include "triangle.hpp"

namespace generator
{

/// A mesh with values evaluated using a callback function.
class parametric_mesh_t
{
public:
	class triangles_t
	{
	public:
		triangle_t generate() const;
		bool done() const noexcept;
		void next();

	private:
		triangles_t(const parametric_mesh_t& mesh);

		const parametric_mesh_t* mesh_;

		gml::ivec2 i_;

		bool even_;

		friend class parametric_mesh_t;
	};

	class vertices_t
	{
	public:
		mesh_vertex_t generate() const;
		bool done() const noexcept;
		void next();

	private:
		vertices_t(const parametric_mesh_t& mesh);

		const parametric_mesh_t* mesh_;

		gml::ivec2 i_;

		friend class parametric_mesh_t;
	};

	/// @param eval A callback that returns a MeshVertex for a given value.
	/// @param segments The number of segments along the surface.
	/// Both should be >= 1. If either is zero an empty mesh is generated.
	explicit parametric_mesh_t(std::function<mesh_vertex_t(const gml::dvec2& t)> eval,
							   const gml::ivec2& segments = {16, 16}) noexcept;

	triangles_t triangles() const noexcept;

	vertices_t vertices() const noexcept;

private:
	std::function<mesh_vertex_t(const gml::dvec2& t)> eval_;

	gml::ivec2 segments_;

	gml::dvec2 delta_;
};
}

#endif
