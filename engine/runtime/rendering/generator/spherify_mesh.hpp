#ifndef SPHERIFYMESH_HPP
#define SPHERIFYMESH_HPP

#include "transform_mesh.hpp"

namespace generator
{

/// Projects vertices on a sphere centered at origin.
template <typename mesh_t>
class spherify_mesh_t
{
private:
	using impl_t = transform_mesh_t<mesh_t>;
	impl_t transform_mesh_;

public:
	spherify_mesh_t(mesh_t mesh, double radius, double factor)
		: transform_mesh_{std::move(mesh), [radius, factor](mesh_vertex_t& value) {
							  value.position =
								  gml::mix(value.position, radius * gml::normalize(value.position), factor);
							  value.normal = gml::normalize(
								  gml::mix(value.normal, gml::normalize(value.position), factor));
						  }}
	{
	}

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return transform_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return transform_mesh_.vertices();
	}
};

template <typename mesh_t>
spherify_mesh_t<mesh_t> spherify_mesh(mesh_t mesh, double radius, double factor)
{
	return spherify_mesh_t<mesh_t>{std::move(mesh), radius, factor};
}
}

#endif
