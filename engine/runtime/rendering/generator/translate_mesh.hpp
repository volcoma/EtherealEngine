#ifndef GENERATOR_TRANSLATEMESH_HPP
#define GENERATOR_TRANSLATEMESH_HPP

#include "transform_mesh.hpp"

namespace generator
{

/// Translates the position of each vertex by given amount.
template <typename mesh_t>
class translate_mesh_t
{
private:
	using impl_t = transform_mesh_t<mesh_t>;
	impl_t transform_mesh_;

public:
	/// @param mesh Source data mesh.
	/// @param delta Amount to increment vertex positions.
	translate_mesh_t(mesh_t mesh, const gml::dvec3& delta)
		: transform_mesh_{std::move(mesh), [delta](mesh_vertex_t& value) { value.position += delta; }}
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
translate_mesh_t<mesh_t> translate_mesh(mesh_t mesh, const gml::dvec3& delta)
{
	return translate_mesh_t<mesh_t>{std::move(mesh), delta};
}
}

#endif
