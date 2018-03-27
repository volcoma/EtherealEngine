#ifndef GENERATOR_SPRINGMESH_HPP
#define GENERATOR_SPRINGMESH_HPP

#include "circle_shape.hpp"
#include "extrude_mesh.hpp"
#include "helix_path.hpp"

namespace generator
{

/// A spring aligned along the z-axis winding counterclockwise
/// @image html SpringMesh.svg
class spring_mesh_t
{
private:
	using impl_t = extrude_mesh_t<circle_shape_t, helix_path_t>;
	impl_t extrude_mesh_;

public:
	/// @param minor Radius of the spring it self.
	/// @param major Radius from the z-axis
	/// @param size Half of the length along the z-axis.
	/// @param slices Subdivisions around the spring.
	/// @param segments Subdivisions along the path.
	/// @param majorStart Counterclockwise angle around the z-axis relative to the x-axis.
	/// @param majorSweep Counterclockwise angle arounf the z-axis.
	spring_mesh_t(double minor = 0.25, double major = 1.0, double size = 1.0, int slices = 8,
				  int segments = 32, double minorStart = 0.0, double minorSweep = gml::radians(360.0),
				  double majorStart = 0.0, double majorSweep = gml::radians(720.0));

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return extrude_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return extrude_mesh_.vertices();
	}
};
}

#endif
