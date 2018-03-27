#ifndef GENERATOR_CAPPEDCYLINDERMESH_HPP
#define GENERATOR_CAPPEDCYLINDERMESH_HPP

#include "cylinder_mesh.hpp"
#include "disk_mesh.hpp"
#include "flip_mesh.hpp"
#include "lathe_mesh.hpp"
#include "line_shape.hpp"
#include "merge_mesh.hpp"
#include "rotate_mesh.hpp"
#include "translate_mesh.hpp"
#include "uv_flip_mesh.hpp"

namespace generator
{

namespace detail
{

class cap_t
{
private:
	using impl_t = translate_mesh_t<disk_mesh_t>;
	impl_t translate_mesh_;

public:
	cap_t(double radius, double distance, int slices, int rings, double start, double sweep);

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

/// Like CylinderMesh but with end caps.
/// @image html CappedCylinderMesh.svg
class capped_cylinder_mesh_t
{
private:
	using impl_t = merge_mesh_t<cylinder_mesh_t, detail::cap_t, uv_flip_mesh_t<flip_mesh_t<detail::cap_t>>>;
	impl_t merge_mesh_;

public:
	/// @param radius Radius of the cylinder along the xy-plane.
	/// @param size Half of the length cylinder along the z-axis.
	/// @param slices Number of subdivisions around the z-axis.
	/// @param segments Number of subdivisions along the z-axis.
	/// @param rings Number of subdivisions on the caps.
	/// @param start Counterclockwise angle around the z-axis relative to x-axis.
	/// @param sweep Counterclockwise angle around the z-axis.
	capped_cylinder_mesh_t(double radius = 1.0, double size = 1.0, int slices = 32, int segments = 8,
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
