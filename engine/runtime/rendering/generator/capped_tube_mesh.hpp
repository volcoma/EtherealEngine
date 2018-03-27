#ifndef GENERATOR_CAPPEDTUBEMESH_HPP
#define GENERATOR_CAPPEDTUBEMESH_HPP

#include "disk_mesh.hpp"
#include "flip_mesh.hpp"
#include "merge_mesh.hpp"
#include "translate_mesh.hpp"
#include "tube_mesh.hpp"

namespace generator
{

namespace detail
{

class tube_cap_t
{
private:
	using impl_t = translate_mesh_t<disk_mesh_t>;
	impl_t translate_mesh_;

public:
	tube_cap_t(double radius, double innerRadius, double distance, int slices, int rings, double start,
			   double sweep);

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

/// Like TubeMesh but with end caps.
/// @image html CappedTubeMesh.svg
class capped_tube_mesh_t
{
private:
	using impl_t = merge_mesh_t<tube_mesh_t, detail::tube_cap_t, flip_mesh_t<detail::tube_cap_t>>;
	impl_t merge_mesh_;

public:
	/// @param radius The outer radius of the cylinder on the xy-plane.
	/// @param innerRadius The inner radius of the cylinder on the xy-plane.
	/// @param size Half of the length of the cylinder along the z-axis.
	/// @param slices Number nubdivisions around the z-axis.
	/// @param segments Number of subdivisions along the z-axis.
	/// @param rings Number radial subdivisions in the cap.
	/// @param start Counterclockwise angle around the z-axis relative to the x-axis.
	/// @param sweep Counterclockwise angle around the z-axis.
	capped_tube_mesh_t(double radius = 1.0, double innerRadius = 0.75, double size = 1.0, int slices = 32,
					   int segments = 8, int rings = 1, double start = 0.0,
					   double sweep = gml::radians(360.0));

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
