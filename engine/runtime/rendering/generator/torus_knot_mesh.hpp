#ifndef GENERATOR_TORUSKNOTMESH_HPP
#define GENERATOR_TORUSKNOTMESH_HPP

#include "circle_shape.hpp"
#include "extrude_mesh.hpp"
#include "knot_path.hpp"

namespace generator
{

/// A Circle extruded along a knot path.
/// @image html TorusKnotMesh.svg
class torus_knot_mesh_t
{
private:
	using impl_t = extrude_mesh_t<circle_shape_t, knot_path_t>;
	impl_t extrude_mesh_;

public:
	/// @param slices Number subdivisions around the circle.
	/// @param segments Number of subdivisions around the path.
	torus_knot_mesh_t(int p = 2, int q = 3, int slices = 8, int segments = 96);

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
