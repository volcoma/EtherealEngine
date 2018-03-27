#ifndef GENERATOR_CAPPEDCONEMESH_HPP
#define GENERATOR_CAPPEDCONEMESH_HPP

#include "cone_mesh.hpp"
#include "disk_mesh.hpp"
#include "flip_mesh.hpp"
#include "merge_mesh.hpp"
#include "translate_mesh.hpp"
#include "uv_flip_mesh.hpp"

namespace generator
{

/// A cone with a cap centered at origin pointing towards positive z-axis.
/// @image html CappedConeMesh.svg
class capped_cone_mesh_t
{
private:
	using impl_t = merge_mesh_t<cone_mesh_t, uv_flip_mesh_t<flip_mesh_t<translate_mesh_t<disk_mesh_t>>>>;
	impl_t merge_mesh_;

public:
	/// @param radius Radius of the flat (negative z) end along the xy-plane.
	/// @param size Half of the length of the cylinder along the z-axis.
	/// @param slices Number of subdivisions around the z-axis.
	/// @param segments Number of subdivisions along the z-axis.
	/// @param rings Number of subdivisions of the cap.
	/// @param start Counterclockwise angle around the z-axis relative to the
	/// positive x-axis.
	/// @param sweep Counterclockwise angle around the z-axis.
	capped_cone_mesh_t(double radius = 1.0, double size = 1.0, int slices = 32, int segments = 8,
					   int rings = 4, double start = 0.0, double sweep = gml::radians(360.0));

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
