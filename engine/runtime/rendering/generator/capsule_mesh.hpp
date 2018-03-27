#ifndef GENERATOR_CAPSULEMESH_HPP
#define GENERATOR_CAPSULEMESH_HPP

#include "cylinder_mesh.hpp"
#include "merge_mesh.hpp"
#include "sphere_mesh.hpp"
#include "translate_mesh.hpp"

namespace generator
{

/// Capsule (cylinder with spherical caps) centered at origin aligned along z-axis.
/// @image html CapsuleMesh.svg
class capsule_mesh_t
{
private:
	using impl_t =
		merge_mesh_t<cylinder_mesh_t, translate_mesh_t<sphere_mesh_t>, translate_mesh_t<sphere_mesh_t>>;
	impl_t merge_mesh_;

public:
	/// @param radius Radius of the capsule on the xy-plane.
	/// @param size Half of the length between centers of the caps along the z-axis.
	/// @param slices Number of subdivisions around the z-axis.
	/// @param rings Number of radial subdivisions in the caps.
	/// @param start Counterclockwise angle relative to the x-axis.
	/// @param sweep Counterclockwise angle.
	capsule_mesh_t(double radius = 1.0, double size = 0.5, int slices = 32, int segments = 4, int rings = 8,
				   double start = 0.0, double sweep = gml::radians(360.0));

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
