#ifndef GENERATOR_SPHERICALCONEMESH_HPP
#define GENERATOR_SPHERICALCONEMESH_HPP

#include "axis_flip_mesh.hpp"
#include "cone_mesh.hpp"
#include "merge_mesh.hpp"
#include "sphere_mesh.hpp"
#include "translate_mesh.hpp"

namespace generator
{

/// A cone with a spherical cap centered at origin tip pointing towards z-axis.
/// Each point on the cap has equal distance from the tip.
/// @image html SphericalConeMesh.svg
class spherical_cone_mesh_t
{
private:
	using impl_t =
		translate_mesh_t<merge_mesh_t<cone_mesh_t, axis_flip_mesh_t<translate_mesh_t<sphere_mesh_t>>>>;
	impl_t translate_mesh_;

public:
	/// @param radius Radius of the negative z end on the xy-plane.
	/// @param size Half of the distance between cap and tip along the z-axis.
	/// @param slices Number of subdivisions around the z-axis.
	/// @param segments Number subdivisions along the z-axis.
	/// @param rings Number subdivisions in the cap.
	/// @param start Counterclockwise angle around the z-axis relative to the positive x-axis.
	/// @param sweep Counterclockwise angle around the z-axis.
	spherical_cone_mesh_t(double radius = 1.0, double size = 1.0, int slices = 32, int segments = 8,
						  int rings = 4, double start = 0.0, double sweep = gml::radians(360.0));

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return translate_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return translate_mesh_.vertices();
	}
};
}

#endif
