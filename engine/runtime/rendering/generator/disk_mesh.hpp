#ifndef GENERATOR_DISKMESH_HPP
#define GENERATOR_DISKMESH_HPP

#include "axis_swap_mesh.hpp"
#include "lathe_mesh.hpp"
#include "line_shape.hpp"

namespace generator
{

/// A circular disk centered at origin on the xy-plane.
/// @image html DiskMesh.svg
class disk_mesh_t
{
private:
	using impl_t = axis_swap_mesh_t<lathe_mesh_t<line_shape_t>>;
	impl_t axis_swap_mesh_;

public:
	/// @param radius Outer radius of the disk on the xy-plane.
	/// @param innerRadius radius of the inner circle on the xy-plane.
	/// @param slices Number of subdivisions around the z-axis.
	/// @param rings Number of subdivisions along the radius.
	/// @param start Counterclockwise angle relative to the x-axis
	/// @param sweep Counterclockwise angle.
	disk_mesh_t(double radius = 1.0, double innerRadius = 0.0, int slices = 32, int rings = 4,
				double start = 0.0, double sweep = gml::radians(360.0));

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
