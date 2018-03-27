#ifndef GENERATOR_CONEMESH_HPP
#define GENERATOR_CONEMESH_HPP

#include "axis_swap_mesh.hpp"
#include "lathe_mesh.hpp"
#include "line_shape.hpp"
#include "uv_flip_mesh.hpp"

namespace generator
{

/// A cone centered at origin tip pointing towards z-axis.
/// @image html ConeMesh.svg
class cone_mesh_t
{
private:
	using impl_t = axis_swap_mesh_t<lathe_mesh_t<line_shape_t>>;
	impl_t axis_swap_mesh_;

public:
	///@param radius Radius of the negative z end on the xy-plane.
	///@param size Half of the length of the cylinder along the z-axis.
	///@param slices Number of subdivisions around the z-axis.
	///@param segments Number subdivisions along the z-axis.
	///@param start Counterclockwise angle around the z-axis relative to the x-axis.
	///@param sweep Counterclockwise angle around the z-axis.
	cone_mesh_t(double radius = 1.0, double size = 1.0, int slices = 32, int segments = 8, double start = 0.0,
				double sweep = gml::radians(360.0));

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
