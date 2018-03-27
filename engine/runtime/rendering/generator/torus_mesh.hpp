#ifndef GENERATOR_TORUS_HPP
#define GENERATOR_TORUS_HPP

#include "axis_swap_mesh.hpp"
#include "circle_shape.hpp"
#include "lathe_mesh.hpp"
#include "translate_shape.hpp"

namespace generator
{

/// Torus centered at origin on the xy-plane.
/// @image html TorusMesh.svg
class torus_mesh_t
{
private:
	using impl_t = axis_swap_mesh_t<lathe_mesh_t<translate_shape_t<circle_shape_t>>>;
	impl_t axis_swap_mesh_;

public:
	/// @param minor Radius of the minor (inner) ring
	/// @param major Radius of the major (outer) ring
	/// @param slices Subdivisions around the minor ring
	/// @param segments Subdivisions around the major ring
	/// @param minorStart Counterclockwise angle relative to the xy-plane.
	/// @param minorSweep Counterclockwise angle around the circle.
	/// @param majorStart Counterclockwise angle around the z-axis relative to the x-axis.
	/// @param majorSweep Counterclockwise angle around the z-axis.
	torus_mesh_t(double minor = 0.25, double major = 1.0, int slices = 16, int segments = 32,
				 double minorStart = 0.0, double minorSweep = gml::radians(360.0), double majorStart = 0.0,
				 double majorSweep = gml::radians(360.0));

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
