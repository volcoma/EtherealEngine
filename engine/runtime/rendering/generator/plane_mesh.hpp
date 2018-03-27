#ifndef GENERATOR_PLANE_HPP
#define GENERATOR_PLANE_HPP

#include "parametric_mesh.hpp"

namespace generator
{

/// A plane (rectangular grid) on the xy -plane normal pointing towards z-axis.
/// @image html PlaneMesh.svg
class plane_mesh_t
{
private:
	using impl_t = parametric_mesh_t;
	impl_t parametric_mesh_;

public:
	/// @param size Half of the side length in x (0) and y (1) direction.
	/// @param segments Number of subdivisions in the x (0) and y (1) direction.
	plane_mesh_t(const gml::dvec2& size = {1.0, 1.0}, const gml::ivec2& segments = {8, 8});

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return parametric_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return parametric_mesh_.vertices();
	}
};
}

#endif
