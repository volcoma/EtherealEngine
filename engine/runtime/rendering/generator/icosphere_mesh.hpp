#ifndef GENERATOR_ICOSPHEREMESH_HPP
#define GENERATOR_ICOSPHEREMESH_HPP

#include "icosahedron_mesh.hpp"
#include "spherify_mesh.hpp"

namespace generator
{

/// Icosphere aka spherical subdivided icosahedron
/// @image html IcoSphereMesh.svg
class ico_sphere_mesh_t
{
private:
	using impl_t = spherify_mesh_t<icosahedron_mesh_t>;
	impl_t spherify_mesh_;

public:
	/// @param radius The radius of the containing sphere.
	/// @param segments The number of segments per icosahedron edge. Must be >= 1.
	ico_sphere_mesh_t(double radius = 1.0, int segments = 4);

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return spherify_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return spherify_mesh_.vertices();
	}
};
}

#endif
