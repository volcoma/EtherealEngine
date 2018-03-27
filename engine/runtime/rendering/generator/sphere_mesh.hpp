#ifndef GENERATOR_SPHERE_HPP
#define GENERATOR_SPHERE_HPP

#include "axis_swap_mesh.hpp"
#include "circle_shape.hpp"
#include "lathe_mesh.hpp"
#include "uv_flip_mesh.hpp"

namespace generator
{

/// A sphere of the given radius centered around the origin.
/// Subdivided around the z-axis in slices and along the z-axis in segments.
/// @image html SphereMesh.svg
class sphere_mesh_t
{
private:
	using impl_t = axis_swap_mesh_t<lathe_mesh_t<circle_shape_t>>;
	impl_t axis_swap_mesh_;

public:
	/// @param radius The radius of the sphere
	/// @param slices Subdivisions around the z-azis (longitudes).
	/// @param segments Subdivisions along the z-azis (latitudes).
	/// @param sliceStart Counterclockwise angle around the z-axis relative to x-axis.
	/// @param sliceSweep Counterclockwise angle.
	/// @param segmentStart Counterclockwise angle relative to the z-axis.
	/// @param segmentSweep Counterclockwise angle.
	sphere_mesh_t(double radius = 1.0, int slices = 32, int segments = 16, double sliceStart = 0.0,
				  double sliceSweep = gml::radians(360.0), double segmentStart = 0.0,
				  double segmentSweep = gml::radians(180.0));

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return axis_swap_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return axis_swap_mesh_.vertices();
	}
};
}

#endif
