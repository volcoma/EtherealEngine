#ifndef GENERATOR_TUBEMESH_HPP
#define GENERATOR_TUBEMESH_HPP

#include "cylinder_mesh.hpp"
#include "flip_mesh.hpp"
#include "merge_mesh.hpp"
#include "uv_flip_mesh.hpp"

namespace generator
{

/// Tube (thick cylinder) centered at origin aligned along the z-axis.
/// @image html TubeMesh.svg
class tube_mesh_t
{
private:
	using impl_t = merge_mesh_t<cylinder_mesh_t, flip_mesh_t<uv_flip_mesh_t<cylinder_mesh_t>>>;
	impl_t merge_mesh_;

public:
	/// @param radius The outer radius of the cylinder on the xy-plane.
	/// @param innerRadius The inner radius of the cylinder on the xy-plane.
	/// @param size Half of the length of the cylinder along the z-axis.
	/// @param slices Subdivisions around the z-axis.
	/// @param segments Subdivisions along the z-axis.
	/// @param start Counterclockwise angle around the z-axis relative to the x-axis.
	/// @param sweep Counterclockwise angle around the z-axis.
	tube_mesh_t(double radius = 1.0, double innerRadius = 0.75, double size = 1.0, int slices = 32,
				int segments = 8, double start = 0.0, double sweep = gml::radians(360.0));

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return merge_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return merge_mesh_.vertices();
	}
};
}

#endif
